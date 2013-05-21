#ifndef ROXLU_OPENGL_FILTER_DEMO_H
#define ROXLU_OPENGL_FILTER_DEMO_H

#include <glr/Shader.h>
#include <experimental/Filter.h>

#define ERR_GL_DEMO_SHADER "Cannot create shader"

namespace gl {

  static const char* DEMO_FILTER_VS = GLSL(120, 
                                           attribute vec4 a_pos;
                                           attribute vec2 a_tex;
                                           varying vec2 v_tex;
                                           void main() {
                                             gl_Position = a_pos;
                                             v_tex = a_tex;
                                           }
  );

  static const char* DEMO_FILTER_FS = GLSL(120, 
                                           uniform sampler2D u_tex;
                                           varying vec2 v_tex;
                                           void main() {
                                             vec4 col = texture2D(u_tex, v_tex);
                                             gl_FragColor = col;
                                             gl_FragColor.r += 0.2;
                                           }
  );

class DemoFilter : public Filter {
 public:
  bool setup(int w, int h); 
  void render(); 
 public:
  gl::Shader shader;
  GLuint u_tex;
};

} // namespace gl
#endif
