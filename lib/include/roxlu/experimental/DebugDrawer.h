#ifndef ROXLU_DEBUG_DRAWERH
#define ROXLU_DEBUG_DRAWERH

#ifdef ROXLU_GL_WRAPPER 

#include <roxlu/Roxlu.h>
#include <string>

const std::string DEBUG_VS = ""
  "uniform mat4 u_projection_matrix;"
  "uniform mat4 u_view_matrix;"
  "attribute vec4 a_pos;"
  "attribute vec4 a_col;"
  "varying vec4 v_col; "
  ""
  "void main() { "
  "  gl_Position = u_projection_matrix * u_view_matrix  * a_pos; "
  "  v_col = a_col; "
  " } ";

const std::string DEBUG_FS = ""
  "varying vec4 v_col; "
  "void main() { "
  "  gl_FragColor = v_col; "
  "}";


const std::string DEBUG_TEX_VS = ""
  "uniform mat4 u_projection_matrix;"
  "uniform mat4 u_model_matrix; "
  "attribute vec4 a_pos;"
  "attribute vec2 a_tex;"
  "varying vec2 v_tex; "
  ""
  "void main() { "
  "  gl_Position = u_projection_matrix * u_model_matrix * a_pos; "
  "  v_tex = a_tex; "
  " } ";

const std::string DEBUG_TEX_FS = ""
  "uniform sampler2D u_tex;"
  "uniform float u_border_mix;"
  "uniform vec4 u_border_col; "
  "varying vec2 v_tex; "
  "void main() { "
  "  vec4 col = texture2D(u_tex, v_tex); "
  "  gl_FragColor = col; "
  "}";
//   "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); "
struct DrawEntry {
  size_t start_dx;
  size_t num_elements;
  GLenum type;
};

class DebugDrawer {
 public:
  DebugDrawer();
  ~DebugDrawer();
  void begin(GLenum type);
  void end();
  void addVertex(const Vec3 pos, const Vec4 col);
  void addVertex(const float x, const float y, const float z, float r = 1.0f, float g = 0.0f, float b = 0.0f);
  void draw(const float* pm, const float* vm);
  void drawTexture(GLuint tex, const float x, const float y, const float w, const float h);
 private: 
  void checkSize();
  void createOrtho(int ww, int wh);
  void setupOpenGL();
  size_t getNumBytes();
  const float* getPtr();
                 
 private:
  bool is_initialized;
  bool needs_update;
  std::vector<VertexPC> vertices;
  std::vector<DrawEntry> entries;
  DrawEntry entry;
  VertexPC vertex;

  int win_w;
  int win_h;
  
  // Texture drawing
  GLuint shader_tex;
  GLuint vbo_tex;
  GLuint vao_tex;
  GLuint prog_tex;
  GLuint pm_id_tex; /* proj matrix */
  GLuint mm_id_tex; /* model matrix */
  GLuint tex_uniform; /* texture uniform id */
  float ortho_pm[16];
  float mm_tex[16]; /* texture model matrix */

  // Generic drawing members
  // --------
  size_t bytes_allocated;
  GLuint pos_id;
  GLuint col_id;
  GLuint projection_matrix_id;
  GLuint view_matrix_id;
  GLuint vbo;
  GLuint vao;
  GLuint vert_id;
  GLuint frag_id;
  GLuint prog_id;
};
#endif // ROXLU_GL_WRAPPER
#endif
