#ifndef ROXLU_BMFONT_RENDERER_H
#define ROXLU_BMFONT_RENDERER_H

#include <vector>
#include <iterator>
#include <roxlu/Roxlu.h>
#include <bmfont/BMTypes.h>

#define BMF_ERR_NOT_SETUP "You didn't call setup so we cant render the BMFont"
#define BMF_ERR_NOT_ALLOC "We havent allocated any data for the GPU yet, did you call update() and added vertices?"
#define BMF_ERR_NO_IMAGE "The given BMFontLoader object couldnt find the font image. Dit you call BMFontLoader::load() before calling BMFontRenderer::setup()"
#define BMF_ERR_NO_IMAGE_FOUND "Cannot find the image file."
#define BMF_ERR_WRONG_NCOMPONENTS "Wrong number of image channels in image file."
#define BMF_ERR_IMAGE_SIZE "The loaded image width/height is not the same as defined in the font"

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

class BMFontLoader;

class BMFontRenderer {
 public:
  BMFontRenderer(BMFontLoader& font);
  ~BMFontRenderer();
  void setup(int windowW, int windowH);                                   /* setup the renderer; we need to the windowW/H for the ortho graphic projection matrix */
  void update();                                                          /* updates the VBO if needed */
  void addVertices(std::vector<BVertex>& vertices);                       /* add vertices to the VBO */
  void draw();                                                            /* render all strings */
  void reset();                                                           /* reset the VBO, call this when you are updating the text repeatedly */
 private:          
  void clear();                                                           /* deallocates everything and resets the complete state; */
  
 private:
  bool is_setup;
  BMFontLoader& font;

  static GLuint prog;
  static GLuint vao;
  static GLint u_projection_matrix;
  static GLint u_tex;

  GLuint vbo;
  GLuint tex;

  float projection_matrix[16];
  std::vector<GLint> multi_firsts;
  std::vector<GLsizei> multi_counts;
  std::vector<BVertex> vertices;
  size_t bytes_allocated;
};

#endif
