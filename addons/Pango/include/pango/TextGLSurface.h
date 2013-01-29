/** 
 * TextGLSurface
 * ------------- 
 * Used to render a TextSurface to screen for GL. This class is optmized 
 * for fast texture uploads, and so changing text elements. If you have static
 * text you can use an normal opengl texture to render. Use: text.getPixels() to 
 * retrieve the pixels then.
 * 
 */

#ifndef ROXLU_TEXT_GL_SURFACE_H
#define ROXLU_TEXT_GL_SURFACE_H

#include <roxlu/Roxlu.h>
#include <pango/TextSurface.h>
#include <string>

//#define TEXT_GL_SURFACE_TIMER 
#if defined(TEXT_GL_SURFACE_TIMER)
#   define TEXT_GL_SURFACE_TIMER_START rx_uint64 now = rx_millis();
#   define TEXT_GL_SURFACE_TIMER_END   rx_uint64 d = rx_millis() - now; printf("Text surface timer: %lld\n", d);
#else
#   define TEXT_GL_SURFACE_TIMER_START
#   define TEXT_GL_SURFACE_TIMER_END
#endif

#define TEXT_GL_SURFACE_USE_PBOS
#define TEXT_GL_SURFACE_NUM_PBOS 2
//#define TEXT_GL_SURFACE_USE_NDC_COORDS /* use normalized device coordinates */

#if defined(TEXT_GL_SURFACE_USE_NDC_COORDS)
static const char* TEXT_GL_SURFACE_VS = GLSL(120, 
  attribute vec4 a_pos;
  attribute vec2 a_tex;
  varying vec2 v_tex;
  uniform mat4 u_pm;
  uniform mat4 u_mm;
  void main() {
   gl_Position = u_mm * a_pos;
   v_tex = a_tex;
  }
);
#else 
static const char* TEXT_GL_SURFACE_VS = GLSL(120, 
  attribute vec4 a_pos;
  attribute vec2 a_tex;
  varying vec2 v_tex;
  uniform mat4 u_pm;
  uniform mat4 u_mm;
  void main() {
   gl_Position = u_pm * u_mm * a_pos;
   v_tex = a_tex;
  }
);
#endif

static const char* TEXT_GL_SURFACE_FS = GLSL(120, 
  uniform sampler2DRect u_tex;
  varying vec2 v_tex;
  void main() {
    gl_FragColor = texture2DRect(u_tex, v_tex);
  }
);


class TextGLSurface {
 public:
  TextGLSurface();
  ~TextGLSurface();
  void setup(const std::string font, unsigned int surfaceW, unsigned int surfaceH, unsigned int windowW, unsigned int windowH);
  void setPixels(unsigned char* pixels);
  void setText(const std::string str, float r = 0.0f, float g = 0.0f, float b = 1.0f, float a = 1.0f);
  void fill(float r, float g, float b, float a);
  void draw(int x, int y);
  void clear();
 public:
  TextSurface text;
 private:
  static GLuint prog;
  static GLint u_pm;
  static GLint u_mm;
  static GLint u_tex;
  static GLfloat pm[16];
  static bool pm_set;

  GLuint tex;
  GLuint vao;
  GLuint vbo;
  Mat4 mm;
  unsigned int surface_w;
  unsigned int surface_h;
  unsigned int window_w;
  unsigned int window_h;
  size_t num_bytes;

  GLuint pbos[TEXT_GL_SURFACE_NUM_PBOS];
  unsigned int read_dx;
  unsigned int write_dx;

  bool needs_update;
  bool is_setup;
  bool has_text;
  bool pbos_filled;
};

inline void TextGLSurface::setText(const std::string str, float r, float g, float b, float a) {
  text.setText(str, r, g, b, a);
  has_text = true;
  needs_update = true;
}

inline void TextGLSurface::fill(float r, float g, float b, float a) {
  text.fill(r,g,b,a);
  needs_update = true;
}

inline void TextGLSurface::clear() {
  text.clear();
  needs_update = true;
}
#endif

