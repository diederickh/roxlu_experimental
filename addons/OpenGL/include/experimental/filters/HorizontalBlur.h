#ifndef ROXLU_OPENGL_HORIZONTAL_BLUR_FILTER_DEMO_H
#define ROXLU_OPENGL_HORIZONTAL_BLUR_FILTER_DEMO_H

#include <glr/Shader.h>
#include <experimental/Filter.h>

#define ERR_GL_VBLUR_SHADER "Cannot create blur shader"

namespace gl {

  static const char* HORIZONTAL_BLUR_FILTER_VS = GLSL(120, 
                                                    attribute vec4 a_pos;
                                                    attribute vec2 a_tex;
                                                    varying vec2 v_tex;
                                                    void main() {
                                                      gl_Position = a_pos;
                                                      v_tex = a_tex;
                                                    }
  );

  static const char* HORIZONTAL_BLUR_FILTER_FS = GLSL(120, 
                                                      uniform float u_weight[10];
                                                      uniform float u_pix_offset[10];
                                                      uniform float u_width;
                                                      uniform sampler2D u_tex;
                                                      varying vec2 v_tex;

                                                      void main() {
                                                        float dx = 1.0 / u_width;
                                                        vec4 sum = texture2D(u_tex, v_tex) * u_weight[0];
                                                        for(int i = 1; i < 10; ++i) {
                                                          sum += texture2D(u_tex, v_tex + vec2(u_pix_offset[i] * dx, 0.0) ) * u_weight[i];
                                                          sum += texture2D(u_tex, v_tex - vec2(u_pix_offset[i] * dx, 0.0) ) * u_weight[i] ;
                                                        }
                                                        gl_FragColor = sum;
                                                      }
  );

  class HorizontalBlur : public Filter {
  public:
    HorizontalBlur(float amount);
    bool setup(int w, int h);
    void render();
    float gauss(float x, float sigma2);
  public:
    gl::Shader shader;
    GLuint u_tex;
    GLuint u_weight;
    GLuint u_width;
    GLuint u_pix_offset;
    float blur_amount;
  };

} // namespace gl
#endif
