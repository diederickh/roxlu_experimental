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

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/core/Utils.h>
#include <bmfont/BMFTypes.h>

#if defined(ROXLU_GL_CORE3)

static const char* BMFONT_VS = GLSL(150, 
                                    uniform mat4 u_projection_matrix;
                                    in vec4 a_pos;
                                    in vec2 a_tex;
                                    in vec4 a_fg_color;

                                    out vec2 v_tex;
                                    out vec4 v_fg_color;

                                    void main() {
                                      gl_Position = u_projection_matrix * a_pos;
                                      v_tex = a_tex;
                                      v_fg_color = a_fg_color;
                                    }
);

static const char* BMFONT_FS = GLSL(150, 
                                    uniform sampler2DRect u_tex;
                                    in vec2 v_tex;
                                    in vec4 v_fg_color;
                                    out vec4 fragcolor;

                                    void main() {
                                      vec4 col = texture(u_tex, v_tex);
                                      fragcolor = col.r * v_fg_color; 
                                      fragcolor.a = 1.0;
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
                                    varying vec2 v_tex;
                                    varying vec4 v_fg_color;
                                    

                                    void main() {
                                      vec4 col = texture2DRect(u_tex, v_tex);
                                      gl_FragColor = col.r * v_fg_color; 
                                      gl_FragColor.a = 1.0;

                                    }
);


#endif

// ---------------------------------------

class BMFShader {
 public:
  BMFShader();
  ~BMFShader();

  virtual void setup(GLuint vbo, GLuint tex);
  virtual void draw(const float* projectMatrix); 

 public:
  static GLuint prog;
  static GLuint vao;
  static GLint u_projection_matrix;
  static GLint u_tex;
  GLuint tex;
};

#endif 

