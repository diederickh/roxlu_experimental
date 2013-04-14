/* 
   RenderPass
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

#define ERR_RPASS_FBO_SIZE "The FBO size is invalid"
#define ERR_RPASS_ADD_FILTERS "You can only create the render pass after you've added filters!"
#define ERR_RPASS_UNI_MM "Cannot find uniform for model matrix"
#define ERR_RPASS_UNI_VM "Cannot find uniform for view matrix"
#define ERR_RPASS_UNI_PM "Cannot find uniform for projection matrix"

namespace gl {


  class Filter;

  class RenderPass {
  public:
    RenderPass();
    ~RenderPass();

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float *vm);
    void setModelMatrix(const float* mm);

    void addFilter(Filter* f);
    bool create(int fboW, int fboH);
    void render();

  private:
    bool setupShaders();
    bool setupBuffers();

  public:
    GLuint prog;
    GLint u_pm;
    GLint u_vm;
    GLint u_mm;
    GLint u_col;

    GLuint fbo;
    int fbo_w; 
    int fbo_h;

  public:
    std::vector<Filter*> filters;

  };

  inline void RenderPass::setViewMatrix(const float* vm) {
    assert(prog);

    glUseProgram(prog);
    glUniformMatrix4fv(u_vm, 1, GL_FALSE, vm);
  }

  inline void RenderPass::setProjectionMatrix(const float* pm) {
    assert(prog);

    glUseProgram(prog);
    glUniformMatrix4fv(u_pm, 1, GL_FALSE, pm);
  }

  inline void RenderPass::setModelMatrix(const float* mm) {
    assert(prog);
    glUniformMatrix4fv(u_mm, 1, GL_FALSE, mm);
  }

} // gl 
#endif
