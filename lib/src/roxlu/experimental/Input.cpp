#ifdef ROXLU_GL_WRAPPER 

#include <roxlu/experimental/Input.h>
#include <roxlu/experimental/Font.h>
#include <roxlu/core/platform/Platform.h>

#define STB_TEXTEDIT_STRING Input
#define STB_TEXTEDIT_STRINGLEN(obj) rx_textedit_string_length(obj)
#define STB_TEXTEDIT_LAYOUTROW(r, obj, n) rx_textedit_layout_row(r, obj, n)
#define STB_TEXTEDIT_GETWIDTH(obj, n, i) rx_textedit_get_width(obj, n, i)
#define STB_TEXTEDIT_KEYTOTEXT(k) rx_textedit_key_to_text(k)
#define STB_TEXTEDIT_GETCHAR(obj, i) rx_textedit_get_char(obj, i)
#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_DELETECHARS(obj, i, n) rx_textedit_delete_chars(obj, i, n)
#define STB_TEXTEDIT_INSERTCHARS(obj, i, c, n) rx_textedit_insert_chars(obj, i, c, n)

#define STB_TEXTEDIT_K_SHIFT  1 << 10
//(0x11000)

// We need to set these to a sane value
#if ROXLU_GL_WRAPPER == ROXLU_GLFW
#include <glfw_opengl/glfw3.h>
//#define STB_TEXTEDIT_K_SHIFT 0
#define STB_TEXTEDIT_K_LEFT GLFW_KEY_LEFT
#define STB_TEXTEDIT_K_RIGHT GLFW_KEY_RIGHT
#define STB_TEXTEDIT_K_UP GLFW_KEY_UP
#define STB_TEXTEDIT_K_DOWN GLFW_KEY_DOWN
#define STB_TEXTEDIT_K_LINESTART GLFW_KEY_HOME
#define STB_TEXTEDIT_K_LINEEND GLFW_KEY_END
#define STB_TEXTEDIT_K_TEXTSTART GLFW_KEY_PAGE_UP
#define STB_TEXTEDIT_K_TEXTEND GLFW_KEY_PAGE_DOWN
#define STB_TEXTEDIT_K_DELETE GLFW_KEY_DELETE
#define STB_TEXTEDIT_K_BACKSPACE GLFW_KEY_BACKSPACE
#define STB_TEXTEDIT_K_UNDO GLFW_KEY_Z | GLFW_KEY_LEFT_CONTROL
#define STB_TEXTEDIT_K_REDO GLFW_KEY_Z | GLFW_KEY_LEFT_ALT
#else 
//#error "Input is only implemented for GLFW now..."
#endif

#define F32 float
#define S16 short int
#define S32 int


#if ROXLU_GL_WRAPPER == ROXLU_GLFW

#define STB_TEXTEDIT_IMPLEMENTATION
#include <roxlu/external/stb_textedit.h>

namespace roxlu {
  // ------------- 
  int rx_textedit_string_length(Input* input) {
    printf("string_length() : %ld\n", input->text.size());
    return input->text.size();
  }

  void rx_textedit_layout_row(StbTexteditRow* r, Input* input, int c) {
    printf("layout_row(): ///////////////////////  %d\n", c);
  }

  float rx_textedit_get_width(Input* input, int start, int end) {
    printf("get_width() -----------------------------------------------   \n");
    return 5;
  }

  unsigned char rx_textedit_key_to_text(char k) {
    printf("to_text(), %c\n",k);
    return (unsigned char) k;
  }

  char rx_textedit_get_char(Input* input, int dx) {

    printf("get_char(), %d()\n", dx);
    return input->text.at(dx);
  }

  void rx_textedit_delete_chars(Input* input, int start, int end) {
    input->text.erase(start, end);
    printf("delete_chars() +++++++++++++++++++++++++++++++++++: %d, %d: %s \n", start, end, input->text.c_str());
    input->font->write(input->entry, input->text);
  }

  int rx_textedit_insert_chars(Input* input, int start, char* chars, int n) {
    printf("IS IN INSERT MODE: %02X, cursor: %d, %02X\n", input->state.insert_mode, input->state.cursor, input->state.single_line);
    //    input->text.insert(input->text.size() - start, chars, n);
    input->text.insert( start, chars, n);
    input->font->write(input->entry, input->text);
    printf("insert_chars(), start: %d, length: %d, char: %s, new text is: %s\n", start, n,  chars, input->text.c_str());
    return 0;
  }

  // --------------

  Input::Input() 
    :is_initialized(false)
    ,is_shader_created(false)
    ,vbo(0)
  {
  
    //  stb_textedit_initialize_state
    stb_textedit_initialize_state(&state, 1);
  }

  Input::Input(Font* font, unsigned int entry) {
    setFont(font, entry);
  }

  Input::~Input() {
  }

  void Input::setFont(Font* f, unsigned int dx) {
    font = f;
    entry = dx;
    is_initialized = true;

    if(!is_shader_created) {
      printf("------\n%s\n-----------", INPUT_VS.c_str());
      shader.create(INPUT_VS, INPUT_FS);
      shader.a("a_pos", 0);
      shader.link();
      shader.u("u_projection_matrix").u("u_model_matrix");
      
      // @todo, calculate carret size
      float cw = 6.0f * 0.5;  // draw from center
      float ch = f->getSize() * 0.7; // draw from center
      printf("SIZE: %d, %f\n", f->getSize(), ch);
      float rect[] = {
        0, 0.0f,
        cw, 0.0f,
        cw, -ch, 

        cw, -ch,
        0, -ch,
        0, 0.0f
      };
      is_shader_created = true;
      vao.bind();
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
      
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0);
    }
  }

  void Input::updateCarretPosition() {
    carret_x = font->getStringWidth(text, 0, state.cursor);
  }

  void Input::draw() {
    updateCarretPosition();
    font->draw();
    vao.bind();
    shader.enable();
    

    FontEntry& e = (*font)[entry];
    Mat4 model;
    model.setPosition(e.pos[0] + carret_x, e.pos[1], -0.1f);

    shader.uniformMat4fv("u_projection_matrix", font->getPM());
    shader.uniformMat4fv("u_model_matrix", model.getPtr());

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  void Input::onMouseDown(int x, int y) {
    if(!is_initialized) {
      return;
    }
    FontEntry& e = (*font)[entry];
    float xx = (x - e.pos[0]);
    printf("X: %f\n", xx);
    stb_textedit_click(this, &state,  x - e.pos[0], y - e.pos[1]); // local x/y
  }

  void Input::onMouseDragged(int x, int y) {
    if(!is_initialized) {
      return;
    }
    FontEntry& e = (*font)[entry];
    stb_textedit_drag(this, &state, e.pos[0] - x, e.pos[1] - y); // local x/y
  }

  void Input::onKeyDown(int key) {
    if(!is_initialized) {
      return;
    }
    stb_textedit_key(this, &state, key); // local/xy
    
  }

} // roxlu
#endif // ROXLU_GL_WRAPPER

#endif // ROXLU_GL_WRAPPER
