#ifndef ROXLU_OPENGL_COMPOSITOR_H
#define ROXLU_OPENGL_COMPOSITOR_H

#include <assert.h>
#include <vector>
#include <roxlu/opengl/GL.h>
#include <experimental/Filter.h>
#include <experimental/filters/Demo.h>
#include <experimental/filters/VerticalBlur.h>
#include <experimental/filters/HorizontalBlur.h>
#include <glr/Mesh.h>
#include <glr/FBO.h>

#define ERR_RPASS_FBO_SIZE "The FBO size is invalid"
#define ERR_RPASS_ADD_FILTERS "You can only create the render pass after you've added filters!"
#define ERR_RPASS_FBO_SETUP "Cannot setup the FBO"
#define ERR_RPASS_FULLSCREEN_SHADER "Cannot create the fullscreen shader"

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
                                               uniform sampler2D u_tex;
                                               varying vec2 v_tex;
                                               void main() {
                                                 vec4 col = texture2D(u_tex, v_tex);
                                                 gl_FragColor =  col;
                                               }
  );

  class Compositor {
  public:
    Compositor();
    ~Compositor();

    void setProjectionMatrix(const float* pm);
    void setViewMatrix(const float *vm);
    void setModelMatrix(const float* mm);

    bool create(int fboW, int fboH);
    void addFilter(Filter* filter);

    void begin();
    void end();
    void draw();

  private:
    bool setupShaders();
    bool setupBuffers();
    bool setupFBO(int fboW, int fboH);

  public:
    //    gl::FBO fbo;
    gl::Mesh<VertexPT> fullscreen_quad;
    gl::Shader fullscreen_shader;
    GLuint fullscreen_u_tex;
    GLuint textures[2];
    GLenum buffers[2];
    GLuint fbo;
    GLuint depth;
    int fbo_w;
    int fbo_h; 

    const float* pm;
    const float* vm;
    const float* mm;

    std::vector<Filter*> filters;
    //    std::vector<Texture*> textures; // ping - pong textures
    //    std::vector<GLenum> buffers; // GL_COLOR_ATTACHMENT0 / 1, just handy for ping-ponging

    int buffer_dx;
    //int texture_dx; 
    int final_buffer;

  };

  inline void Compositor::setProjectionMatrix(const float* m) {
    pm = m;
  }

  inline void Compositor::setViewMatrix(const float* m) {
    vm = m;
  }

  inline void Compositor::setModelMatrix(const float* m) {
    mm = m;
  }

} // namespace gl

#endif
