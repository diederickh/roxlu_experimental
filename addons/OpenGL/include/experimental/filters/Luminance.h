#ifndef ROXLU_OPENGL_FILTER_LUMINANCE_H
#define ROXLU_OPENGL_FILTER_LUMINANCE_H

#include <glr/Shader.h>
#include <experimental/Filter.h>

#define ERR_GL_LUMINANCE_SHADER "Cannot create luminance shader"

namespace gl {

  static const char* LUMINANCE_FILTER_VS = GLSL(120, 
                                                attribute vec4 a_pos;
                                                attribute vec2 a_tex;
                                                varying vec2 v_tex;
                                                void main() {
                                                  gl_Position = a_pos;
                                                  v_tex = a_tex;
                                                }
  );

  static const char* LUMINANCE_FILTER_FS = GLSL(120, 
                                                uniform sampler2D u_tex;
                                                varying vec2 v_tex;
                                                void main() {
                                                  vec4 col = texture2D(u_tex, v_tex);
                                                  if(col.r > 0.2) {
                                                    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
                                                  }
                                                  else {
                                                    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
                                                  }
                                                }
  );

  class Luminance : public Filter {
  public:
    bool setup(int w, int h); 
    void render(); 
    int getNumberOfNeededColorAttachments();
  public:
    gl::Shader shader;
    GLuint u_tex;
  };

} // namespace gl
#endif
