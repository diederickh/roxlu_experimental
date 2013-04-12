#ifndef ROXLU_OPENGL_VAO_H
#define ROXLU_OPENGL_VAO_H

#include <glr/VBO.h>
#include <glr/Vertex.h>
#include <roxlu/opengl/GL.h>

#define ERR_GL_VAO_ALREADY_SETUP "The VAO has been setup already"
#define ERR_GL_VAO_NOT_SETUP "The VAO is not setup"
#define ERR_GL_VAO_CANNOT_CREATE "Cannot create the VAO object"
#define ERR_GL_VAO_VBO_NOT_BOUND "No vbo bound. Make sure a VBO is bound before enabling attributes"
#define ERR_GL_VAO_VERTEX_NOT_SUPPORTED "The vertex types is not supported by the VAO"

namespace gl {

  class VAO {
  public:
    VAO();
    ~VAO();

    bool setup();                                  /* sets up the VAO and enables the attributes, and sets the vertex attrib pointers. make sure that the VBO that you want to use with this VBO is bound! */

    void enableAttributes(VBO<VertexP>& vbo);      /* enables the correct attributes for VertexP  */
    void enableAttributes(VBO<VertexPT>& vbo);     /* enables the correct attributes for VertexPT */
    void enableAttributes(VBO<VertexCP>& vbo);     /* enables the correct attributes for VertexCP */
    void enableAttributes(VBO<VertexNP>& vbo);     /* enables the correct attributes for VertexNP */

    void bind();
    void unbind();

  public:
    GLuint id;
  };

} // gl 
#endif
