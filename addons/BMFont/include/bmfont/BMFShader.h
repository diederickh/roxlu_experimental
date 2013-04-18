/* 

   BMFShader
   ---------
   The BMFShader class is used to setup a VAO and shader for the 
   used vertex type. Note that all the BMF* types are temlated on 
   the vertex type because this allows you to use the BMFont type 
   for other kind of vertices if you would need that.

   By default, when you don't explicitly pass the shader to `BMFFont::setup()`
   we will allocate this BMFShader() for you and handle all memory management.
   The default `BMFShader` can be used for both GL 2.x and 3.x
   
 */

#ifndef ROXLU_BMFONT_SHADER_H
#define ROXLU_BMFONT_SHADER_H

#include <roxlu/opengl/GL.h>
#include <roxlu/core/Utils.h>
#include <bmfont/BMFTypes.h>

#if defined(ROXLU_GL_CORE3)

static const char* BMFONT_VS = GLSL(150, 
                                    uniform mat4 u_projection_matrix;
                                    uniform mat4 u_model_matrix;
                                    in vec4 a_pos;
                                    in vec2 a_tex;
                                    in vec4 a_fg_color;

                                    out vec2 v_tex;
                                    out vec4 v_fg_color;

                                    void main() {
                                      gl_Position = u_projection_matrix * u_model_matrix * a_pos;
                                      v_tex = a_tex;
                                      v_fg_color = a_fg_color;
                                    }
);

static const char* BMFONT_FS = GLSL(150, 
                                    uniform sampler2DRect u_tex;
                                    uniform float u_alpha;
                                    in vec2 v_tex;
                                    in vec4 v_fg_color;
                                    out vec4 fragcolor;

                                    void main() {
                                      float intensity = texture(u_tex, v_tex).r;
                                      fragcolor = vec4(v_fg_color.rgb, intensity) * u_alpha;
                                    }
);

#else 
static const char* BMFONT_VS = GLSL(120, 
                                    uniform mat4 u_projection_matrix;
                                    attribute vec4 a_pos;
                                    attribute vec2 a_tex;
                                    attribute vec4 a_fg_color;

                                    varying vec2 v_tex;
                                    varying vec4 v_fg_color;

                                    void main() {
                                      gl_Position = u_projection_matrix * a_pos;
                                      v_tex = a_tex;
                                      v_fg_color = a_fg_color;
                                    }
);

static const char* BMFONT_FS = GLSL(120, 
                                    uniform sampler2DRect u_tex;
                                    uniform float u_alpha;
                                    varying vec2 v_tex;
                                    varying vec4 v_fg_color;

                                    void main() {
                                      float intensity = texture2DRect(u_tex, v_tex).r;
                                      gl_FragColor = vec4(v_fg_color.rgb, intensity) * u_alpha;
                                    }
);


#endif

// ---------------------------------------

class BMFShader {
 public:
  BMFShader();
  ~BMFShader();

  virtual void setup(GLuint vbo, GLuint tex);
  virtual void setModelMatrix(const float* modelMatrix);
  virtual void setProjectMatrix(const float* projectionMatrix);
  virtual void setAlpha(float a);
  virtual void draw();

 public:
  static GLuint prog;
  static GLint u_projection_matrix;
  static GLint u_model_matrix;
  static GLint u_tex;
  static GLint u_alpha;
  GLuint vao;
  GLuint tex;
};

inline void BMFShader::setModelMatrix(const float* mm) {
  glUseProgram(prog);
  glUniformMatrix4fv(u_model_matrix, 1, GL_FALSE, mm);
}

inline void BMFShader::setProjectMatrix(const float* pm) {
  glUseProgram(prog);
  glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE, pm);
}

inline void BMFShader::setAlpha(float a) {
  glUseProgram(prog);
  glUniform1f(u_alpha, a);
}

#endif 

