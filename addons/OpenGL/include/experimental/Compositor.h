/* 
   Compositor
   -----------

   Generic pipeline for adding simple effects to a render pass. Because
   you can easily add effects to a render pass we need to have a couple
   of standards. For example we create different render-buffers based, 
   on the added filters. For example a light ray effect would need a 
   occlusion buffer, if you add this we will automatically allocate the 
   nessary space for this buffer. The shader needs to know into what buffer
   it needs to write though... and therefore we fix the gl_FragData locations:

   - Location `0` is used to render the diffuse color (so textures etc..)
   - Location `1` is used to render the occlusion buffer (e.g. used by light ray filter)

*/

#ifndef ROXLU_OPENGL_RENDERPASS_H
#define ROXLU_OPENGL_RENDERPASS_H

#include <assert.h>
#include <vector>
#include <roxlu/opengl/GL.h>
#include <glr/Mesh.h>
#include <glr/FBO.h>

#define ERR_RPASS_FBO_SIZE "The FBO size is invalid"
#define ERR_RPASS_ADD_FILTERS "You can only create the render pass after you've added filters!"
#define ERR_RPASS_FBO_SETUP "Cannot setup the FBO"
#define ERR_RPASS_FULLSCREEN_SHADER "Cannot create the fullscreen shader"

//#define ERR_RPASS_UNI_MM "Cannot find uniform for model matrix"
//#define ERR_RPASS_UNI_VM "Cannot find uniform for view matrix"
//#daefine ERR_RPASS_UNI_PM "Cannot find uniform for projection matrix"

namespace gl {

  static const char* COMP_FULLSCREEN_VS = GLSL(120,
                                               attribute vec4 a_pos;
                                               attribute vec2 a_tex;
                                               varying vec2 v_tex;
                                               void main() {
                                                 gl_Position = a_pos;
                                                 v_tex = a_tex;
                                               }
  );

  static const char* COMP_FULLSCREEN_FS = GLSL(120,
                                               uniform sampler2D u_tex0;
                                               varying vec2 v_tex;
                                               void main() {
                                                 vec4 col = texture2D(u_tex0, v_tex);
                                                 gl_FragColor =  col;
                                               }
  );
                                               

  // ----------------------------------------------

  class Filter;

  // ----------------------------------------------

  struct Pass {
    Pass();

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float* vm);
    void setModelMatrix(const float* mm);

    int num;

    GLuint prog;
    GLint u_pm;
    GLint u_vm;
    GLint u_mm;
    GLint u_col;
    
  };

  struct RenderPass : public Pass {
    RenderPass();
  };

  struct ShaderPass : public Pass {
    ShaderPass();
  };
  
  // ----------------------------------------------

  class Compositor {
  public:
    Compositor();
    ~Compositor();

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float *vm);
    void setModelMatrix(const float* mm);

    bool create(int fboW, int fboH);
    bool createPass(Pass& pass);
    void render();
    void renderPass(Pass& pass);
    int countShaderPasses();       /* returns the number of shader passes we need to apply */

    void addFilter(Filter* f);
    bool getShaderSource(Filter* f, GLenum shader, int pass, std::string& global, std::string& main);

    bool createDrawBuffer(std::string& fragdata, std::string& uniform); /* this will create a new texure and attaches this to a free color attachment in the FBO we use to render into.  The fragdata is set to the fragdata entry you should use in your shader; it will have a value like: `gl_FragData[2]`.  The uniform is set to the value of the texture sampler that will be used to bind the color-attachment in a shader pass (note: not render pass). It returns false on error */

  private:
    bool setupShaders();
    bool setupBuffers();
    bool setupFBO(int fboW, int fboH);

  public:
    FBO fbo;
    RenderPass render_pass;
    std::vector<Filter*> filters;
    std::vector<ShaderPass*> shader_passes;

    Mesh<VertexPT> fullscreen_quad;
    gl::Shader fullscreen_shader;
  };

  inline void Compositor::setViewMatrix(const float* vm) {
    render_pass.setViewMatrix(vm);
  }

  inline void Compositor::setProjectionMatrix(const float* pm) {
    render_pass.setProjectionMatrix(pm);
  }

  inline void Pass::setViewMatrix(const float* vm) {
    assert(prog);
    assert(vm);
    assert(u_vm >= 0);

    glUseProgram(prog);
    glUniformMatrix4fv(u_vm, 1, GL_FALSE, vm);
  }

  inline void Pass::setProjectionMatrix(const float* pm) {
    assert(prog);
    assert(pm);
    assert(u_pm >= 0);

    glUseProgram(prog);
    glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
  }


  inline void Pass::setModelMatrix(const float* mm) {
    assert(prog);
    assert(mm);
    assert(u_mm >= 0);

    glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);
  }

} // gl 
#endif
