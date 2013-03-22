#ifndef ROXLU_FPS_H
#define ROXLU_FPS_H

#include <roxlu/opengl/Shader.h>
#include <roxlu/opengl/GL.h>
#include <roxlu/Roxlu.h>

#include <string>

#if defined(ROXLU_WITH_OPENGL)

namespace roxlu { 
#if defined(ROXLU_GL_CORE3)
  static const char* FPS_VS = GLSL(150, 
                                   uniform mat4 u_pm;
                                   uniform mat4 u_vm;
                                   in vec4 a_pos;
                                   void main() {
                                     gl_Position = u_pm * u_vm * a_pos;
                                   }
  );

  static const char* FPS_FS = GLSL(150, 
                                   uniform vec4 u_col;
                                   out vec4 outcol;
                                   void main() {
                                     outcol = u_col;
                                     outcol = vec4(1.0, 0.0, 0.0, 1.0);
                                   }
  );

#else 
  const std::string FPS_VS = ""
    "uniform mat4 u_pm;"
    "uniform mat4 u_mm;"
    "attribute vec4 a_pos; "
    ""
    "void main() { " 
    "   gl_Position = u_pm * u_mm * a_pos; "
    "}";

  const std::string FPS_FS = ""
    "uniform vec4 u_col; "
    "void main() {"
    "   gl_FragColor = u_col; "
    "}";
#endif

  struct FPS_Element {
    FPS_Element();
    void set(unsigned int s, unsigned int num);
    int start;
    int num_vertices;
  };

  enum FPS_Align {
    FA_TOP_LEFT,
    FA_TOP_RIGHT,
    FA_BOTTOM_RIGHT,
    FA_BOTTOM_LEFT
  };

  class FPS {
  public:
    FPS(int align = FA_BOTTOM_RIGHT, float scale = 2.0f); // scale 1 == 4 pixels width, 7 pixels height
    ~FPS();
    void draw();
    void setColor(const float r, const float g, const float b, const float a = 1.0f);
  private:
    void setupGL();
    void createOrtho(int w, int h);
  private:
    bool is_setup;
    unsigned int fps;
    unsigned int frame_count;
    rx_uint64 timeout;
    Shader shader;
    GLuint vao;
    GLuint vbo;
    FPS_Element elements[20];
    float pm[16];
    float mm[16]; 
    float fg_col[4];
    int win_w;
    int win_h;
    unsigned int draw_list[4];
    int align;
    float scale;
    float x;
    float y;
  };

  inline void FPS::setColor(const float r, const float g, const float b, const float a) {
    fg_col[0] = r;
    fg_col[1] = g;
    fg_col[2] = b;
    fg_col[3] = a;
  }
  
} // roxlu

#endif // ROXLU_WITH_OPENGL
#endif
