#include <experimental/Compositor.h>
#include <experimental/Filter.h>
#include <glr/Texture.h>

namespace gl {

  Compositor::Compositor() 
    :buffer_dx(0)
  {
  }

  Compositor::~Compositor() {
  }

  bool Compositor::create(int fboW, int fboH) {
    if(!filters.size()) {
      RX_ERROR(ERR_RPASS_ADD_FILTERS);
      return false;
    }

    if(!setupFBO(fboW, fboH)) {
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


  bool Compositor::setupFBO(int fboW, int fboH) {
    fbo_w = fboW;
    fbo_h = fboH;

    if(!fboW || !fboH) {
      RX_ERROR(ERR_RPASS_FBO_SIZE);
      return false;
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // COLOR ATTACHMENTS
    glGenTextures(2, textures);

    for(int i = 0; i < 2; ++i) {                
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboW, fboH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, textures[i], 0);
    }

    // DEPTH
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fboW, fboH);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      (*it)->setup(fbo_w, fbo_h);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffers[0] = GL_COLOR_ATTACHMENT0;
    buffers[1] = GL_COLOR_ATTACHMENT1;

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
  
  void Compositor::addFilter(Filter* filter) {
    filters.push_back(filter);
  }

  void Compositor::begin() {
    glViewport(0, 0, fbo_w, fbo_h);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // CLEAR ALL ATTACHMENTS
    glDrawBuffers(2, buffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // CAPTURE SCENE INTO 0
    GLenum buf[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buf);
  }

  void Compositor::end() {

    fullscreen_quad.bind();

    glDepthMask(GL_FALSE);
    buffer_dx = 1;

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* f = *it;
      glDrawBuffer(buffers[buffer_dx]);
      buffer_dx = 1 - buffer_dx;
      f->render(textures[buffer_dx]);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    buffer_dx = 1 - buffer_dx;

    // RESET 
    glDepthMask(GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK_LEFT);
  }


  void Compositor::draw() {
    fullscreen_shader.use();
    fullscreen_quad.bind();
    
    glDepthMask(GL_FALSE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[buffer_dx]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDepthMask(GL_TRUE);
  }

} // namespace gl
