#include <sstream>
#include <experimental/Compositor.h>
#include <experimental/Filter.h>
#include <glr/Texture.h>
#include <roxlu/core/Utils.h>

namespace gl {

  // ----------------------------------------------
  Pass::Pass()
    :num(-1)
    ,prog(0)
    ,u_pm(-1)
    ,u_vm(-1)
    ,u_mm(-1)
    ,u_col(-1)
  {
  }

  RenderPass::RenderPass()
    :Pass()
  {
  }

  ShaderPass::ShaderPass() 
    :Pass()
  {
  }

  // ----------------------------------------------

  Compositor::Compositor() {
    render_pass.num = 0; 
  }

  Compositor::~Compositor() {
    RX_ERROR("NEED TO DELETE THE SHADERPASSES");
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

  bool Compositor::setupFBO(int fboW, int fboH ) {
    if(!fboW || !fboH) {
      RX_ERROR(ERR_RPASS_FBO_SIZE);
      return false;
    }

    if(!fbo.setup(fboW, fboH)) {
      RX_ERROR(ERR_RPASS_FBO_SETUP);
      return false;
    }

    if(!fbo.addTexture(GL_COLOR_ATTACHMENT0)) {
      RX_ERROR(ERR_RPASS_FBO_SETUP);
      return false;
    }

    if(!fbo.addDepthBuffer()) {
      RX_ERROR(ERR_RPASS_FBO_SETUP);
      return false;
    }

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* filter = *it;
      filter->createDrawBuffers(*this);
    }
    
    return true;
  }

  bool Compositor::createDrawBuffer(std::string& fragdata, std::string& uniform) {
    size_t num = fbo.getNumTextures();

    if(fbo.addTexture(GL_COLOR_ATTACHMENT0 + num)) {

      std::stringstream fss;
      fss << "gl_FragData[" << num << "]";
      fragdata = fss.str();

      std::stringstream uss; 
      uss << "u_tex" << num;
      uniform = uss.str();

      return true;
    }
    return false;
  }

  bool Compositor::setupShaders() {

    if(!fullscreen_shader.create(COMP_FULLSCREEN_VS, COMP_FULLSCREEN_FS)) {
      RX_ERROR(ERR_RPASS_FULLSCREEN_SHADER);
      return false;
    }
    fullscreen_shader.bindAttribLocation("a_pos", 0);
    fullscreen_shader.bindAttribLocation("a_tex", 1);
    fullscreen_shader.link();

    if(!createPass(render_pass)) {
      RX_ERROR("Cannot create main render pass");
      return false;
    }
    
    int num_shader_passes = countShaderPasses();
    for(int i = 0; i < num_shader_passes; ++i) {
      ShaderPass* pass = new ShaderPass();
      pass->num = i + 1; // the first pass is the render pass
      shader_passes.push_back(pass);
      if(!createPass(*pass)) {
        RX_ERROR("CANNOT CREATE SHADER PASS!");
      }

    }
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


  bool Compositor::createPass(Pass& pass) {

    if(pass.num < 0) {
      RX_ERROR("Render pass not intialized!");
      return false;
    }

    std::string vert_shader_global = "" 
      "\n"
      "uniform mat4 u_pm;              // projection matrix \n"
      "uniform mat4 u_vm;              // view matrix \n"
      "uniform mat4 u_mm;              // model matrix \n"
      "attribute vec4 a_pos;           // attrib 0 \n"
      "attribute vec2 a_tex;           // attrib 1 \n"
      "attribute vec4 a_col;           // attrib 2 \n"
      "attribute vec3 a_norm;          // attrib 3 \n"
      "varying vec4 v_col;             // varying color, to frag shader \n"
      "varying vec3 v_norm;            // varying normal (as input), to frag shader \n"
      "varying vec2 v_tex;             // varying texture coordinate (as input), to frag shader \n"
     "\n";

    std::string frag_shader_global = ""
      "\n"
      "uniform vec4 u_col;             // color for the color buffer (not diffuse buffer) \n"
      "uniform sampler2D u_tex0;       // diffuse color buffer \n"
      "varying vec4 v_col;             // color per vertex \n"
      "varying vec3 v_norm;            // normal \n"
      "varying vec2 v_tex;             // texcoord \n"
      "\n";

    std::string vert_shader_main = ""; 
    std::string frag_shader_main = "";
    std::string shader_global;
    std::string shader_main;

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {

      Filter* filter = *it;

      if(getShaderSource(filter, GL_VERTEX_SHADER, pass.num, shader_global, shader_main)) {

        if(shader_global.size()) {
          vert_shader_global += shader_global;
          shader_global.clear();
        }
        
        if(shader_main.size()) {
          vert_shader_main += shader_main;
          shader_main.clear();
        }
      }

      if(getShaderSource(filter, GL_FRAGMENT_SHADER, pass.num, shader_global, shader_main)) {

        if(shader_global.size()) {
          frag_shader_global += shader_global;
          shader_global.clear();
        }

        if(shader_main.size()) {
          frag_shader_main += shader_main;
          shader_main.clear();
        }
      }

    }

    std::string position = "";
    if(pass.num == 0) {
      position = "  gl_Position = u_pm * u_vm * u_mm * a_pos; \n";
    }
    else {
      position = "  gl_Position = a_pos; \n";
    }

    // STITCH SHADERS TOGETHER
    std::string vs = vert_shader_global +""
      "\n"
      "void main() {  \n"
      +position
      +"  v_norm = a_norm; \n"
      "  v_col = a_col; \n"
      "  v_tex = a_tex; \n"
      +vert_shader_main +""
      "}\n";

    std::string fs = frag_shader_global +""
      "\n"
      "void main() { \n"
      +frag_shader_main +""
      "}\n";

    printf("%s", vs.c_str());
    printf("%s", fs.c_str());

    pass.prog = rx_create_shader(vs.c_str(), fs.c_str());

    glBindAttribLocation(pass.prog, 0, "a_pos");
    glBindAttribLocation(pass.prog, 1, "a_tex");
    glBindAttribLocation(pass.prog, 2, "a_col");
    glBindAttribLocation(pass.prog, 3, "a_norm");
    glLinkProgram(pass.prog);
    
    glUseProgram(pass.prog);
    pass.u_vm = glGetUniformLocation(pass.prog, "u_vm");
    pass.u_pm = glGetUniformLocation(pass.prog, "u_pm");
    pass.u_mm = glGetUniformLocation(pass.prog, "u_mm");
    pass.u_col = glGetUniformLocation(pass.prog, "u_col");

    printf("prog: %d\n", pass.prog);

    assert(pass.prog);
    //    assert(pass.u_vm >= 0);
    //    assert(pass.u_pm >= 0);
    //    assert(pass.u_mm >= 0);

    return true;
  }

  bool Compositor::getShaderSource(Filter* filter, GLenum shader, int pass, std::string& global, std::string& main) {
    assert(pass >= 0);

    if(pass == 0) {
      return filter->getRenderPassSource(shader, global, main);
    }
    else if(pass > 0) {
      return filter->getShaderPassSource(shader, pass, global, main);
    }

    return false;
  }

  void Compositor::addFilter(Filter* f) {
    filters.push_back(f);
  }

  void Compositor::render() {
    // RENDER PASSES
    glViewport(0,0,fbo.width, fbo.height);
    fbo.bind();
    fbo.clear();
    fbo.drawBuffers(fbo.getColorAttachments());
    
    renderPass(render_pass);

    fbo.unbind();
  
    // SHADER PASSES
    std::vector<Texture*> textures = fbo.getTextures();
    //glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_BACK_LEFT);
    for(std::vector<ShaderPass*>::iterator it = shader_passes.begin(); it != shader_passes.end(); ++it) {
      ShaderPass* shader_pass = *it;
      glUseProgram(shader_pass->prog);

      for(size_t i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]->getID());
        std::stringstream ss;
        ss << "u_tex" << i;
        GLint uni = glGetUniformLocation(shader_pass->prog, ss.str().c_str());
        if(uni >= 0) {
          glUniform1i(uni, i);
        }
      }

      fullscreen_quad.bind();
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    /*
    Texture* tex = fbo.getTexture(GL_COLOR_ATTACHMENT0);
    assert(tex);
    
    // RESULT
    fullscreen_shader.activeTexture(*tex, GL_TEXTURE0);
    fullscreen_shader.use();
    fullscreen_quad.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    */

  }

  void Compositor::renderPass(Pass& pass) {
    assert(pass.prog);
    assert(pass.num >= 0);

    glUseProgram(pass.prog);
    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* f = *it;
      f->render(pass);
    }
  }

  int Compositor::countShaderPasses() {

    int num_shader_passes = 0;

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* f = *it;
      if(f->getNumShaderPasses() > num_shader_passes) {
        num_shader_passes = f->getNumShaderPasses();
      }
    }

    return num_shader_passes;
  }

} // gl
