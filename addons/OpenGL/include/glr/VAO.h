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

  template<class T>
  class VAO {
  public:
    VAO();
    ~VAO();

    bool setup();                                  /* sets up the VAO and enables the attributes, and sets the vertex attrib pointers. make sure that the VBO that you want to use with this VBO is bound! */
    void enableAttributes(VBO<T>& vbo);
    void bind();
    void unbind();

  public:
    GLuint id;
  };

  // ---
  template<class T>
  VAO<T>::VAO()
    :id(0)
  {
  }

  template<class T>
  VAO<T>::~VAO() {
    id = 0;
  }

  template<class T>
  bool VAO<T>::setup() { 
    
    if(id) {
      RX_ERROR(ERR_GL_VAO_ALREADY_SETUP);
      return false;
    }

    glGenVertexArrays(1, &id);

    if(!id) {
      RX_ERROR(ERR_GL_VAO_CANNOT_CREATE);
      return false;
    }

    return true;
  }

  template<class T>
  void VAO<T>::enableAttributes(VBO<T>& vbo) {
    bind();
    vbo.bind();

    glr_vao_enable_attributes(vbo);
    
    unbind();
    
  }

  template<class T>
  void VAO<T>::bind() {
   
    if(!id) {
      RX_ERROR(ERR_GL_VAO_NOT_SETUP);
      return;
    }

    glBindVertexArray(id);
  }

  template<class T>
  void VAO<T>::unbind() {
    glBindVertexArray(0);
  }

  // ---

  inline void glr_vao_enable_attributes(VBO<VertexP>& vbo) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (GLvoid*)0);
  }

  inline void glr_vao_enable_attributes(VBO<VertexPT>& vbo) {
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // tex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)0); // pos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)12); // tex
  }

  inline void glr_vao_enable_attributes(VBO<VertexCP>& vbo) {
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(2); // col
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCP), (GLvoid*)16); // pos
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCP), (GLvoid*)0); // col
  }

  inline void glr_vao_enable_attributes(VBO<VertexNP>& vbo) {
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(3); // norm
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNP), (GLvoid*)12); // pos
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNP), (GLvoid*)0); // norm
  }


} // gl 
#endif
