#ifndef ROXLU_STB_FONTH
#define ROXLU_STB_FONTH

#ifdef ROXLU_GL_WRAPPER

/*
Status: 

[X] font baking
[X] texture setup
[X] shader setup
[X] vbo setup
[X] vao setup
[X] drawing
[X] alignment of text (?)
[X] update test 

 */

#include <roxlu/Roxlu.h>
#include <stdio.h>

#undef STB_TRUETYPE_IMPLEMENTATION
#include <roxlu/external/stb_truetype.h>

const std::string FONT_VS = " \
uniform mat4 u_projection_matrix;                             \
uniform mat4 u_model_matrix;                                  \
attribute vec4 a_pos;                                         \
attribute vec2 a_tex;                                         \
attribute vec4 a_col;                                         \
attribute vec4 a_bg;                                          \
varying vec2 v_tex;                                           \
varying vec4 v_col;                                           \
varying vec4 v_bg;                                            \
void main() {                                                 \
  v_tex = a_tex;                                              \
  v_col = a_col;                                              \
  v_bg = a_bg;                                                \
  gl_Position = u_projection_matrix * u_model_matrix * a_pos; \
}                                                             \
";

const std::string FONT_FS = " \
uniform sampler2D u_font_texture;                                 \
varying vec2 v_tex;                                               \
varying vec4 v_col;                                               \
varying vec4 v_bg;                                                \
void main() {                                                     \
   vec4 col = texture2D(u_font_texture, v_tex);                   \
   gl_FragColor = v_col;                                          \
   gl_FragColor.a = col.a * v_col.a;                              \
}                                                                 \
";

/* gl_FragColor = mix(v_col, v_bg, col.a); use for bg col */   
/* gl_FragColor.a = col.r * u_col.a;   */  

namespace roxlu {
  enum FontEntryAlign {
    FEA_LEFT,
    FEA_RIGHT  
  };

  struct FontEntry {
    FontEntry();
    float pos[2];
    float col[4];
    size_t start_dx;
    size_t num_vertices;
    int w; // width of the text
    int align;
    float right_edge; // when alignment is FEA_RIGHT, this is the edge to which we align
    bool needs_update; // set to true when e.g. position, with, vertices etc.. changes.
  };
  
  struct CharVertex {
    CharVertex();
    CharVertex(float x, float y, float s, float t, const float* txtCol); // txtCol & bgCol are float[4]
    float pos[2];
    float tex[2];
    float col[4];
    float bg[4]; // we implemented a background color, but this is the bg per char so we're not really using it
  };

  class Font {
  public:
    Font();
    ~Font();
    bool open(const std::string& filepath, unsigned int size);
    int write(const std::string& text, float x, float y, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
    int write(unsigned int dx, const std::string& text, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f); // update text entry @index
    void alignRight(unsigned int dx, float rightEdge); 
    unsigned int getSize();
    void update();
    void draw();
    void flagChanged();
    float getStringWidth(const std::string& str, unsigned int start, unsigned int end);
    const float* getPM(); // get the projection matrix
    FontEntry& operator[](const unsigned int dx);
  private: 
    void updateEntryIndices();
    void setupGL(unsigned char* bakedBitmap, int bw, int bh);
    void createOrtho(int winW, int winH);
    const float* getPtr(); // get ptr to first CharVertex.pos.x
    size_t getNumBytes(); // number of bytes needed for VBO

  private:
    bool needs_update;
    int win_w;
    int win_h;
    int w; // font texture w
    int h; // font texture h
    int size; // font size in pixels
    stbtt_bakedchar* cdata;
    bool is_initialized;
  
    // openGL
    float mm[16];
    float pm[16];
    Shader shader;
    GLuint tex;
    VAO vao;
    GLuint vbo;
    std::vector<FontEntry> entries;
    std::vector<CharVertex> vertices;
    size_t allocated_bytes;
  };
  
  inline const float* Font::getPtr() {
    return &vertices[0].pos[0];
  }

  inline size_t Font::getNumBytes() {
    return vertices.size() * sizeof(CharVertex);
  }

  inline void Font::flagChanged() {
    needs_update = true;
  }

  inline FontEntry& Font::operator[](const unsigned int dx) {
    return entries[dx];
  }

  inline unsigned int Font::getSize() {
    return size;
  }

  inline const float* Font::getPM() {
    return pm;
  }

} // roxlu

#endif // ROXLU_GL_WRAPPER
#endif
