#ifdef ROXLU_GL_WRAPPER

#ifndef ROXLU_AXISH
#define ROXLU_AXISH

#include <string>
#include <roxlu/Roxlu.h>

/*
#ifdef GL_ES
precision highp float;
#endif
*/

namespace roxlu {

  const std::string AXIS_VS = " "
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "uniform mat4 u_projection_matrix; "
    "uniform mat4 u_view_matrix; "
    "attribute vec4 a_pos; "
    "attribute vec3 a_col; "
    "varying vec3 v_col; "
    ""
    "void main() { "
    "  gl_Position = u_projection_matrix * u_view_matrix * a_pos; "
    "  v_col = a_col; "
    " } "
    "";

  const std::string AXIS_FS = " "
    "#ifdef GL_ES\n"
    "  precision highp float;\n"
    "#endif\n"
    ""
    "varying vec3 v_col; "
    "void main() { "
    "  gl_FragColor.a = 1.0; "
    "  gl_FragColor.rgb = v_col; "
    "}"
    "";

  class Axis {
  public:
    Axis();
    void setup(int size);
    void draw(const float* pm, const float* vm);
  private:
    GLuint vbo;
    VAO vao;
    Shader shader;
    size_t nvertices;
  };

} // roxlu
#endif

#endif // ROXLU_GL_WRAPPER
