#ifndef ROXLU_OPENGL_DRAWER_H
#define ROXLU_OPENGL_DRAWER_H

#include <roxlu/opengl/GL.h>
#include <glr/VAO.h>
#include <glr/VBO.h>
#include <glr/Mesh.h>
#include <glr/Texture.h>
#include <glr/Shader.h>

#define ERR_GL_DR_UNSUPPORTED_VERTEX_TYPE "The given vertex type is not supported"

namespace gl {

  // VERTEX_P
  // ----------------------------
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


  // VERTEX_PT
  // ----------------------------
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
                                       gl_FragColor.r += 0.1;
                                     }
  );


  class Drawer {
  public:
    Drawer();
    ~Drawer();

    void createOrthographicMatrix();
    const float* getOrthographicMatrix();
    
    void onResize(unsigned int w, unsigned int h);                                     /* when the window resizes we need to update the ortho-graphic matrix, this must be called by the user */
    void drawTexture(Texture& tex, float x, float y, float w = 0, float h = 0);        /* draws a texture */
    void drawArrays(Mesh<VertexP>& mesh, GLenum mode, GLint first, GLsizei count);     /* draw a mesh, with mode (GL_TRIANGLES, GL_POINTS, etc..), and start drawing the vertices from loation `begin` and draw `count` vertices */

  private:
    void setupShader(Shader& shader);                                                  /* sets up a shader; sets identity matrices + ortho matrix */

  private:
    gl::Shader shader_p;                                                               /* shader for simple VertexP types; only positions */
    gl::Shader shader_pt;                                                              /* shader used for drawing textures */
    Mat4 ortho_matrix;                                                                 /* orthographic projection */
    unsigned int window_w;                                                             /* viewport width; is retrieved automatically when started, or update when the user calls onResize() */
    unsigned int window_h;                                                             /* viewport height; is retrieved automatically when started, or update when the user calls onResize() */
    Mesh<VertexPT> texture_mesh;                                                       /* basic mesh used to draw textures */
  };


  inline const float* Drawer::getOrthographicMatrix() {
     return ortho_matrix.getPtr();
  }

} // gl

#endif
