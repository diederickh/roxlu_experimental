#include <experimental/RenderPass.h>
#include <experimental/Filter.h>
#include <roxlu/core/Utils.h>

namespace gl {

  RenderPass::RenderPass()
    :fbo_w(0)
    ,fbo_h(0)
    ,prog(0)
    ,fbo(0)
  {
  }

  RenderPass::~RenderPass() {
  }

  bool RenderPass::create(int fboW, int fboH) {
    if(!filters.size()) {
      RX_ERROR(ERR_RPASS_ADD_FILTERS);
      return false;
    }

    if(!fboW || !fboH) {
      RX_ERROR(ERR_RPASS_FBO_SIZE);
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

  bool RenderPass::setupShaders() {
    // GENERIC SHADER
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
     "\n";

    std::string frag_shader_global = ""
      "\n"
      "uniform vec4 u_col;             // color for the color buffer (not diffuse buffer) \n"
      "uniform sampler2D u_tex0;       // diffuse color buffer \n"
      "uniform sampler2D u_tex1;       // occlusion bufffer \n"
      "varying vec4 v_col;             // color per vertex \n"
      "varying vec3 v_norm;            // normal \n"
      "\n";


    std::string vert_shader_main = ""; 
    std::string frag_shader_main = "";

      
    // GET CODE FROM FILTERS
    std::string shader_global;
    std::string shader_main;

    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {

      Filter* f = *it;

      if(f->getVertexShaderSource(shader_global, shader_main)) {

        if(shader_global.size()) {
          vert_shader_global += shader_global;
          shader_global = "";
        }

        if(shader_main.size()) {
          vert_shader_main += shader_main;
          shader_main = "";
        }
      }

      if(f->getFragmentShaderSource(shader_global, shader_main)) {

        if(shader_global.size()) {
          frag_shader_global += shader_global;
          shader_global = "";
        }

        if(shader_main.size()) {
          frag_shader_main += shader_main;
          shader_main = "";
        }
      }

    }

    // STITCH SHADERS TOGETHER
    std::string vs = vert_shader_global +""
      "\n"
      "void main() {  \n"
      "  gl_Position = u_pm * u_vm * u_mm * a_pos; \n"
      "  v_norm = a_norm; \n"
      "  v_col = a_col; \n"
      +vert_shader_main +""
      "}\n";

    std::string fs = frag_shader_global +""
      "\n"
      "void main() { \n"
      +frag_shader_main +""
      "}\n";

    printf("%s", vs.c_str());
    printf("%s", fs.c_str());

    prog = rx_create_shader(vs.c_str(), fs.c_str());
    glBindAttribLocation(prog, 0, "a_pos");
    glBindAttribLocation(prog, 1, "a_tex");
    glBindAttribLocation(prog, 2, "a_col");
    glBindAttribLocation(prog, 3, "a_norm");
    glLinkProgram(prog);
    
    glUseProgram(prog);
    u_vm = glGetUniformLocation(prog, "u_vm");
    u_pm = glGetUniformLocation(prog, "u_pm");
    u_mm = glGetUniformLocation(prog, "u_mm");
    u_col = glGetUniformLocation(prog, "u_col");

    if(u_vm < 0) {
      RX_ERROR(ERR_RPASS_UNI_VM);
      return false;
    }

    if(u_pm < 0) {
      RX_ERROR(ERR_RPASS_UNI_PM);
      return false;
    }

    if(u_vm < 0) {
      RX_ERROR(ERR_RPASS_UNI_VM);
      return false;
    }

    return true;
  }

  bool RenderPass::setupBuffers() {
    return true;
  }

  void RenderPass::addFilter(Filter* f) {
    filters.push_back(f);
  }

  void RenderPass::render() {
    assert(prog);

    glUseProgram(prog);
    for(std::vector<Filter*>::iterator it = filters.begin(); it != filters.end(); ++it) {
      Filter* f = *it;
      f->render(*this);
    }

  }

} // gl
