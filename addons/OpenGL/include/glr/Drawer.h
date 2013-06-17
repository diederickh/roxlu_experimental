/*
  Drawer
  --------

  MAKE SURE THAT YOU CALL `gl_init()` before using any of the openGL addon code

  Vertex attributes are bound to these locations:

  - a_pos  : 0
  - a_tex  : 1
  - a_col  : 2
  - a_norm : 3

 */

#ifndef ROXLU_OPENGL_DRAWER_H
#define ROXLU_OPENGL_DRAWER_H

#include <roxlu/opengl/GL.h>
#include <glr/VAO.h>
#include <glr/VBO.h>
#include <glr/Shader.h>

#define ERR_GL_DR_UNSUPPORTED_VERTEX_TYPE "The given vertex type is not supported"

namespace gl {
#if defined(ROXLU_GL_CORE3)
  // VERTEX_P
  // ---------------------------------------------------
  static const char* GL_VS_P = GLSL(150,
                                    in vec4 a_pos;
                                    uniform mat4 u_pm;
                                    uniform mat4 u_vm;
                                    uniform mat4 u_mm;

                                    void main() {
                                      gl_Position = u_pm * u_vm * u_mm * a_pos;
                                    }
  );

  static const char* GL_FS_P = GLSL(150, 
                                    out vec4 frag_color;
                                    void main() {
                                      frag_color = vec4(0.8, 0.8, 0.8, 1.0);
                                    }
  );


  // VERTEX_CP
  // ---------------------------------------------------
  static const char* GL_VS_CP = GLSL(150,
                                     in vec4 a_pos;
                                     in vec4 a_col;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     out vec4 v_col;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_col = a_col;
                                     }
  );

  static const char* GL_FS_CP = GLSL(150, 
                                     in vec4 v_col;
                                     out vec4 frag_color;
                                     void main() {
                                       frag_color = v_col;
                                       frag_color.a = 1.0;
                                     }
  );


  // VERTEX_PT
  // ---------------------------------------------------
  static const char* GL_VS_PT = GLSL(150,
                                     in vec4 a_pos;
                                     in vec2 a_tex;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     out vec2 v_tex;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_tex = a_tex;
                                     }
  );

  static const char* GL_FS_PT = GLSL(150, 
                                     uniform sampler2D u_tex0;
                                     in vec2 v_tex;
                                     out vec4 frag_color;
                                     void main() {
                                       frag_color = texture(u_tex0, v_tex);
                                     }
  );


  // VERTEX_NP
  // ---------------------------------------------------
  static const char* GL_VS_NP = GLSL(150,
                                     in vec4 a_pos;
                                     in vec3 a_norm;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     out vec3 v_norm;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_norm = a_norm;
                                     }
  );

  static const char* GL_FS_NP = GLSL(150, 
                                     in vec3 v_norm;
                                     out vec4 frag_color;
                                     void main() {
                                       frag_color.rgb = 0.5 * v_norm + 0.5;
                                       frag_color.a = 1.0;
                                     }
  );

#else
  // VERTEX_P
  // ---------------------------------------------------
  static const char* GL_VS_P = GLSL(120,
                                    attribute vec4 a_pos;
                                    uniform mat4 u_pm;
                                    uniform mat4 u_vm;
                                    uniform mat4 u_mm;

                                    void main() {
                                      gl_Position = u_pm * u_vm * u_mm * a_pos;
                                    }
  );

  static const char* GL_FS_P = GLSL(120, 
                                    void main() {
                                      gl_FragColor = vec4(0.8, 0.8, 0.8, 1.0);
                                    }
  );


  // VERTEX_CP
  // ---------------------------------------------------
  static const char* GL_VS_CP = GLSL(120,
                                     attribute vec4 a_pos;
                                     attribute vec4 a_col;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     varying vec4 v_col;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_col = a_col;
                                     }
  );

  static const char* GL_FS_CP = GLSL(120, 
                                     varying vec4 v_col;
                                     void main() {
                                       gl_FragColor = v_col;
                                       gl_FragColor.a = 1.0;
                                     }
  );


  // VERTEX_PT
  // ---------------------------------------------------
  static const char* GL_VS_PT = GLSL(120,
                                     attribute vec4 a_pos;
                                     attribute vec2 a_tex;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     varying vec2 v_tex;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_tex = a_tex;
                                     }
  );

  static const char* GL_FS_PT = GLSL(120, 
                                     uniform sampler2D u_tex0;
                                     varying vec2 v_tex;
                                     void main() {
                                       gl_FragColor = texture2D(u_tex0, v_tex);
                                     }
  );


  // VERTEX_NP
  // ---------------------------------------------------
  static const char* GL_VS_NP = GLSL(120,
                                     attribute vec4 a_pos;
                                     attribute vec3 a_norm;
                                     uniform mat4 u_pm;
                                     uniform mat4 u_vm;
                                     uniform mat4 u_mm;
                                     varying vec3 v_norm;

                                     void main() {
                                       gl_Position = u_pm * u_vm * u_mm * a_pos;
                                       v_norm = a_norm;
                                     }
  );

  static const char* GL_FS_NP = GLSL(120, 
                                     varying vec3 v_norm;
                                     void main() {
                                       gl_FragColor.rgb = 0.5 * v_norm + 0.5;
                                       gl_FragColor.a = 1.0;
                                     }
  );

#endif // ROXLU_GL_CORE3

  // ---------------------------------------------------

  class Texture;

  template<class T>
  class Mesh;

  // ---------------------------------------------------
  
  struct DrawEntry {
    size_t start_dx;                                                                    /* start index in the `Mesh<VertexCP>* immediate_vertices` member of Drawer */
    size_t num_elements;                                                                /* Number of elements/vertices to draw */ 
    GLenum mode;                                                                        /* GL_TRIANGLES, GL_POINTS, etc.. */
  };

  // ---------------------------------------------------

  class Drawer {
  public:
    Drawer();
    ~Drawer();

    void createOrthographicMatrix();
    const float* getOrthographicMatrix();
    void setProjectionMatrix(const float *pm);                                         /* set the projection matrix for the stock shaders: shader_p, shader_pt, shader_### */
    void setViewMatrix(const float* vm);                                               /* set the view matrix for the stock shaders: shader_p, shader_pt, shader_### */
    
    void onResize(unsigned int w, unsigned int h);                                     /* when the window resizes we need to update the ortho-graphic matrix, this must be called by the user */

    void drawCircle(float x, float y, float radius);                                   /* draw a radius at x/y with the given radius */
    void drawRectangle(float x, float y, float w, float h);                            /* draw a  rectangle at top left x/y and w/h */
    void drawTexture(Texture& tex, float x, float y, float w = 0, float h = 0);        /* draws a texture */
    void drawTexture(Texture& tex, const float* mm);                                   /* draws a texture with the given model matrix */
    void drawArrays(Mesh<VertexP>& mesh, GLenum mode, GLint first, GLsizei count);     /* draw a mesh, with mode (GL_TRIANGLES, GL_POINTS, etc..), and start drawing the vertices from loation `begin` and draw `count` vertices */
    void drawArrays(Mesh<VertexPT>& mesh, GLenum mode, GLint first, GLsizei count);    /* draw a mesh, with mode (GL_TRIANGLES, GL_POINTS, etc..), and start drawing the vertices from loation `begin` and draw `count` vertices */
    void drawArrays(Mesh<VertexNP>& mesh, GLenum mode, GLint first, GLsizei count);    /* draw a mesh, with mode (GL_TRIANGLES, GL_POINTS, etc..), and start drawing the vertices from loation `begin` and draw `count` vertices */

    void fill();                                                                       /* immediate mode: draw circle / rectangle with fill */
    void nofill();                                                                     /* immediate mode: draw circle / rectangle without a fill */
    void begin(GLenum mode);                                                           /* immediate mode: begin a sequence of vertices. each vertex is `flushes` after called Drawer::vertex() */
    void color(Vec4 color);                                                            /* immediate mode: set the color for the next vertex */
    void vertex(Vec3 position);                                                        /* immediate mode: end the current sequence of vertices */
    void end();                                                                        /* immediate mode: this will draw the current sequence */
    void draw();                                                                       /* immediate mode: used internally to draw the current sequence... we keep track of all draw entries so we can optimize this in the future */

  private:
    void setupShader(Shader& shader);                                                  /* sets up a shader; sets identity matrices + ortho matrix */

  public:
    gl::Shader shader_p;                                                               /* shader for VertexP types; positions */
    gl::Shader shader_pt;                                                              /* shader for VertexPT types; positions, texture coordinates */
    gl::Shader shader_np;                                                              /* shader for VertexNP types: normals, position */
    gl::Shader shader_tex;                                                             /* shader used for drawing textures */
    gl::Shader shader_immediate;                                                       /* shader used for immediate drawing (vertex(), color(), begin(), end(), draw()) */

    Mat4 ortho_matrix;                                                                 /* orthographic projection */
    unsigned int window_w;                                                             /* viewport width; is retrieved automatically when started, or update when the user calls onResize() */
    unsigned int window_h;                                                             /* viewport height; is retrieved automatically when started, or update when the user calls onResize() */
    Mesh<VertexPT>* texture_mesh;                                                      /* basic mesh used to draw textures */

    DrawEntry immediate_entry;                                                         /* immediate mode: contains information about one draw entry (one set of vertices with e.g. GL_TRIANGLES, GL_LINE_STRIP, GL_POINTS).  */
    Vec3 immediate_color;                                                              /* immediate mode: currently set color; is used for the vertex that is added when you call `Drawer::vertex()` */
    bool immediate_must_fill;                                                          /* immediate mode: must draw rectangle / cricle filled or not */
    Mesh<VertexCP>* immediate_mesh;                                                    /* immediate mode: the VAO/VBO that is contains the vertices we draw */
    std::vector<DrawEntry> immediate_entries;                                          /* immediate mode: all begin()/end() sequences... in the current implmentation this will always be just one entry; but in the future this might be used to optimize */
    std::vector<VertexCP> immediate_circle;                                            /* immediate mode: vertices of a unit circle, used in `Drawer::drawCirlce()` */
  };


 // ---------------------------------------------------

  inline const float* Drawer::getOrthographicMatrix() {
     return ortho_matrix.getPtr();
  }
  
  void glr_init();                                                                     /* must be called if you want to make use of the opengl addon */
  void glr_print_info();                                                               /* print some infomration about the currently active context (like version) */

  void glr_set_view_matrix(const float* vm);                                           /* set the view matrix of the stock shaders: shader_p, shader_pt, etc... see `Drawer::setViewMatrix()` */
  void glr_set_projection_matrix(const float* pm);                                     /* set the projection matrix of the stock shaders: shader_p, shader_pt, etc... see `Drawer::setProjectionMatrix()` */
  const float* glr_get_orthographic_matrix();                                          /* returns a pointer to the stock orthographic projection matrix */

  void glr_draw_circle(float x, float y, float radius);                                /* draw a radius at x/y with the given radius */
  void glr_draw_rectangle(float x, float y, float w, float h);                         /* draw a  rectangle at top left x/y and w/h */

  void glr_fill();                                                                     /* immediate mode: draw circle / rectangle with fill */ 
  void glr_nofill();                                                                   /* immediate mode: draw circle / rectangle without a fill */

  void glr_begin(GLenum mode);                                                         /* immediate mode: begin a vertex sequence */
  void glr_color(Vec4 color);                                                          /* immediate mode: set the color for the next vertex */
  void glr_color(float r, float g, float b, float a = 1.0);                            /* immediate mode: set color */
  void glr_color(int r, int g, int b, int a = 255);                                    /* assign from unsigned char; range 0 - 255 */

  void glr_vertex(Vec3 position);                                                      /* immediate mode: end the current sequence of vertices */
  void glr_end();                                                                      /* immediate mode: this will draw the current sequence */

  Shader& glr_get_shader_np();                                                         /* returns the stock shader for: normals + position, vertex meshes */
  Shader& glr_get_shader_pt();                                                         /* returns the stock shader for: position + texcoord, vertex meshes */

  extern Drawer* glr_context; 

} // gl

#endif
