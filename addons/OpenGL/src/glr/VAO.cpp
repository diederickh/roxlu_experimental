#include <glr/VAO.h>
#include <roxlu/core/Log.h>

namespace gl {

  VAO::VAO()
    :id(0)
  {
  }

  VAO::~VAO() {
    id = 0;
  }

  bool VAO::setup() { 
    
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

  void VAO::enableAttributes(VBO<VertexP>& vbo) {
    bind();
    vbo.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (GLvoid*)0);
    
    unbind();
    
  }

  void VAO::enableAttributes(VBO<VertexPT>& vbo) {
    bind();
    vbo.bind();

    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // tex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)0); // pos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)12); // tex

    vbo.unbind();
    unbind();
  }

  void VAO::enableAttributes(VBO<VertexCP>& vbo) {
    bind();
    vbo.bind();

    glEnableVertexAttribArray(0); // col
    glEnableVertexAttribArray(1); // pos
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCP), (GLvoid*)0); // col
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCP), (GLvoid*)16); // pos

    vbo.unbind();
    unbind();
  }

  void VAO::enableAttributes(VBO<VertexNP>& vbo) {
    bind();
    vbo.bind();

    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(4); // norm
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNP), (GLvoid*)12); // pos
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNP), (GLvoid*)0); // norm

    vbo.unbind();
    unbind();
  }

  void VAO::bind() {
   
    if(!id) {
      RX_ERROR(ERR_GL_VAO_NOT_SETUP);
      return;
    }

    glBindVertexArray(id);
  }

  void VAO::unbind() {
    glBindVertexArray(0);
  }

} // gl
