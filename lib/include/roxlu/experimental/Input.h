#ifndef ROXLU_INPUTH
#define ROXLU_INPUTH

#include <roxlu/Roxlu.h>
#include <string>

#undef STB_TEXTEDIT_IMPLEMENTATION

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_POSITIONTYPE size_t
#define STB_UNDOSTATECOUNT 96
#define STB_UNDOCHARCOUNT 1024

#include <roxlu/external/stb_textedit.h>

namespace roxlu {
  const std::string INPUT_VS = ""
    "uniform mat4 u_projection_matrix;"
    "uniform mat4 u_model_matrix; "
    "attribute vec4 a_pos; "
    ""
    "void main() {"
    "  gl_Position = u_projection_matrix * model_matrix * a_pos; " 
    "}";

  const std::string INPUT_FS = ""
    "void main() {"
    "  gl_FragColor = vec4(1.0); "
    "}";

  
  class Font;
  class Input {
  public:
    Input();
    Input(roxlu::Font* font, unsigned int entry);
    ~Input();

    void setFont(roxlu::Font* font, unsigned int entry);
    void onMouseDown(int x, int y); // global x/y
    void onMouseDragged(int x, int y);  // global x/y
    void onKeyDown(int key);  // global x/y

  public:
    std::string text;
    STB_TexteditState state;
    roxlu::Font* font; // used to render the editable text
    unsigned int entry; // the entry you want to be editable  
  private:
    bool is_shader_created;
    bool is_initialized;
    Shader shader;
    VAO vao;
    GLuint vbo;
  };

  static int rx_textedit_string_length(Input* input);
  static void rx_textedit_layout_row(StbTexteditRow* row, Input* input, int c);
  static float rx_textedit_get_width(Input* input, int start, int end);
  static unsigned char rx_textedit_key_to_text(char k);
  static char rx_textedit_get_char(Input* input, int dx);
  static void rx_textedit_delete_chars(Input* input, int start, int end);
  static int rx_textedit_insert_chars(Input* input, int start, char* chars, int n);
} // roxlu
#endif
