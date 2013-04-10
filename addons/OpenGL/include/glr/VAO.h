#ifndef ROXLU_OPENGL_VAO_H
#define ROXLU_OPENGL_VAO_H

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

    bool setup(unsigned int vertexType);            /* sets up the VAO and enables the attributes, and sets the vertex attrib pointers. make sure that the VBO that you want to use with this VBO is bound! */
    bool enableAttributes();                        /* enables the correct attributes based on the current `type` */
    void bind();
    void unbind();

  public:
    unsigned int vertex_type;
    GLuint id;
  };

} // gl 
#endif
