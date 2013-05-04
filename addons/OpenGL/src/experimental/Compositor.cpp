#include <experimental/Compositor.h>
#include <experimental/Filter.h>
#include <glr/Texture.h>

namespace gl {

  void print_gl_enum(GLenum e) {
    switch(e) {
      case GL_COLOR_ATTACHMENT0: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT0"); break;
      case GL_COLOR_ATTACHMENT1: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT1"); break;
      case GL_COLOR_ATTACHMENT2: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT2"); break;
      case GL_COLOR_ATTACHMENT3: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT3"); break;
      case GL_COLOR_ATTACHMENT4: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT4"); break;
      case GL_COLOR_ATTACHMENT5: RX_VERBOSE("GLenum: GL_COLOR_ATTACHMENT5"); break;
      default: RX_VERBOSE("GLenum: UNKNOWN"); break;
    }
  }

  Compositor::Compositor() 
    :fullscreen_u_tex(-1)
    ,fbo(0)
    ,depth(0)
    ,fbo_w(0)
    ,fbo_h(0)
    ,initialized(false)
  {
  }

  Compositor::~Compositor() {
    // @todo remove all GL objects
  }

  void Compositor::setup(int w, int h) {
    fbo_w = w;
    fbo_h = h;
  }

  bool Compositor::create() {
    if(!filters.size()) {
      RX_ERROR(ERR_RPASS_ADD_FILTERS);
      return false;
    }

    if(!setupFBO()) {
      return false;
    }

    if(!setupShaders()) {
      return false;
    }

    if(!setupBuffers()) {
      return false;
    }

    return true;
  }

  bool Compositor::setupFBO() {

    if(!fbo_w || !fbo_h) {
      RX_ERROR(ERR_RPASS_FBO_SIZE);
      return false;
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // COLOR ATTACHMENTS
    for(std::vector<Attachment>::iterator it = attachments.begin(); it != attachments.end(); ++it) {
      Attachment& a = *it;
      glFramebufferTexture2D(GL_FRAMEBUFFER, a.attachment, GL_TEXTURE_2D, a.texture, 0);
    }

    // DEPTH
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fbo_w, fbo_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      (*it)->setup(fbo_w, fbo_h);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
  }

  bool Compositor::setupShaders() {
    if(!fullscreen_shader.create(COMP_FULLSCREEN_VS, COMP_FULLSCREEN_FS)) {
      RX_ERROR(ERR_RPASS_FULLSCREEN_SHADER);
      return false;
    }

    fullscreen_shader.bindAttribLocation("a_pos", 0);
    fullscreen_shader.bindAttribLocation("a_tex", 1);
    fullscreen_shader.link();

    // setup texture
    fullscreen_u_tex = fullscreen_shader.getUniformLocation("u_tex");
    fullscreen_shader.use();
    glUniform1i(fullscreen_u_tex, 0);

    return true;
  }

  bool Compositor::setupBuffers() {
    VertexPT a(Vec2(-1.0f, -1.0f), Vec2(0.0f, 0.0f));
    VertexPT b(Vec2( 1.0f, -1.0f), Vec2(1.0f, 0.0f));
    VertexPT c(Vec2( 1.0f,  1.0f), Vec2(1.0f, 1.0f));
    VertexPT d(Vec2(-1.0f,  1.0f), Vec2(0.0f, 1.0f));
    
    fullscreen_quad.push_back(a);
    fullscreen_quad.push_back(b);
    fullscreen_quad.push_back(c);
    
    fullscreen_quad.push_back(a);
    fullscreen_quad.push_back(c);
    fullscreen_quad.push_back(d);

    if(!fullscreen_quad.setup(GL_STATIC_DRAW)) {
      return false;
    }

    fullscreen_quad.update();

    return true;
  }

  bool Compositor::getAttachment(int num, Attachment& result) {
    for(std::vector<Attachment>::iterator it = attachments.begin(); it != attachments.end(); ++it) {
      Attachment& a = *it;
      if(a.attachment == GL_COLOR_ATTACHMENT0 + num) {
        result = a;
        return true;
      }
    }
    return false;
  }

  void Compositor::addFilter(Filter* filter, int input, int output) { 
    filters.push_back(filter);

    if(!initialized) {
      createDefaultAttachments();
    }

    Attachment a;
    if(getAttachment(input, a)) {
      filter->setInput(a);
    }
    else {
      RX_VERBOSE("INPUT ATTACHMENT NOT FOUND");
    }

    if(getAttachment(output, a)) {
      filter->setOutput(a);
    }
    else {
      RX_VERBOSE("OUTPUT ATTACHMENT NOT FOUND");
    }
  }

  void Compositor::createAttachment(int num) {
    Attachment a;
    a.attachment = GL_COLOR_ATTACHMENT0 + num;
    a.texture = createTexture();

    attachments.push_back(a);
    buffers.push_back(a.attachment);
    textures.push_back(a.texture);
  }

  GLuint Compositor::createTexture() {
    if(!fbo_w || !fbo_h) {
      RX_ERROR("No fbo_w or fbo_h set.. did you call setup?");
      return 0;
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo_w, fbo_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
  }

  void Compositor::createDefaultAttachments() {

    GLuint tex0 = createTexture();
    GLuint tex1 = createTexture();

    buffers.push_back(GL_COLOR_ATTACHMENT0); // default: ping
    buffers.push_back(GL_COLOR_ATTACHMENT1); // deafult: pong
    textures.push_back(tex0); // texture: ping
    textures.push_back(tex1); // texture: pong

    Attachment ping;
    ping.texture = tex0;
    ping.attachment = GL_COLOR_ATTACHMENT0;
    attachments.push_back(ping);

    Attachment pong;
    pong.texture = tex1;
    pong.attachment = GL_COLOR_ATTACHMENT1;
    attachments.push_back(pong);

    initialized = true;
  }

  void Compositor::begin() {
    glViewport(0, 0, fbo_w, fbo_h);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // CLEAR ALL ATTACHMENTS
    glDrawBuffers(buffers.size(), &buffers[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // CAPTURE SCENE INTO 0
    GLenum buf[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buf);
  }

  void Compositor::end() {
    fullscreen_quad.bind();
    glDepthMask(GL_FALSE);

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* f = *it;
      f->render();
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // RESET 
    glDepthMask(GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK_LEFT);
  }


  void Compositor::draw(int num) {
    Attachment a;
    if(!getAttachment(num, a)) {
      RX_VERBOSE("Cannot find the attachment you want to draw");
      return;
    }

    fullscreen_shader.use();
    fullscreen_quad.bind();
    
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, a.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDepthMask(GL_TRUE);
  }

} // namespace gl
