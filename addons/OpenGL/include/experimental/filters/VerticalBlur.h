#ifndef ROXLU_OPENGL_VERTICAL_BLUR_FILTER_DEMO_H
#define ROXLU_OPENGL_VERTICAL_BLUR_FILTER_DEMO_H

#include <glr/Shader.h>
#include <experimental/Filter.h>

#define ERR_GL_VBLUR_SHADER "Cannot create blur shader"

namespace gl {

  static const char* VERTICAL_BLUR_FILTER_VS = GLSL(120, 
                                                    attribute vec4 a_pos;
                                                    attribute vec2 a_tex;
                                                    varying vec2 v_tex;
                                                    void main() {
                                                      gl_Position = a_pos;
                                                      v_tex = a_tex;
                                                    }
  );

  static const char* VERTICAL_BLUR_FILTER_FS = GLSL(120, 
                                                    uniform float u_weight[10];
                                                    uniform float u_pix_offset[10];
                                                    uniform float u_height;
                                                    uniform sampler2D u_tex;
                                                    varying vec2 v_tex;

                                                    void main() {
                                                      float dy = 1.0 / u_height;
                                                      vec4 sum = texture2D(u_tex, v_tex) * u_weight[0];
                                                      for(int i = 1; i < 10; ++i) {
                                                        sum += texture2D(u_tex, v_tex + vec2(0.0, u_pix_offset[i] * dy) ) * u_weight[i];
                                                        sum += texture2D(u_tex, v_tex - vec2(0.0, u_pix_offset[i] * dy) ) * u_weight[i] ;
                                                      }
                                                      gl_FragColor = sum;
                                                    }
  );

  class VerticalBlur : public Filter {
  public:
    VerticalBlur(float amount);
    bool setup(int w, int h);
    void render(); 
    float gauss(float x, float sigma2);
  public:
    gl::Shader shader;
    GLuint u_tex;
    GLuint u_weight;
    GLuint u_height;
    GLuint u_pix_offset;
    float blur_amount;
  };

} // namespace gl
#endif
