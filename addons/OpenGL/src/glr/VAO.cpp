#include <glr/VAO.h>
#include <roxlu/core/Log.h>

namespace gl {

  VAO::VAO()
    :id(0)
    ,vertex_type(VERTEX_NONE)
  {
  }

  VAO::~VAO() {
    id = 0;
    vertex_type = VERTEX_NONE;
  }

  bool VAO::setup(unsigned int vertexType) {
    
    if(id) {
      RX_ERROR(ERR_GL_VAO_ALREADY_SETUP);
      return false;
    }

    glGenVertexArrays(1, &id);

    if(!id) {
      RX_ERROR(ERR_GL_VAO_CANNOT_CREATE);
      return false;
    }
    
    this->vertex_type = vertexType;
    return true;
  }

  bool VAO::enableAttributes() {

    if(vertex_type == VERTEX_NONE) {
      RX_ERROR(ERR_GL_VAO_NOT_SETUP);
      return false;
    }
    
    if(!id) {
      RX_ERROR(ERR_GL_VAO_NOT_SETUP);
      return false;
    }

    GLint vbo = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
    if(vbo == 0) {
      RX_ERROR(ERR_GL_VAO_VBO_NOT_BOUND);
      return false;
    }

    bind();
    
    if(vertex_type == VERTEX_P) {
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), (GLvoid*)0);
    }
    else if(vertex_type == VERTEX_PT) {
      glEnableVertexAttribArray(0); // pos
      glEnableVertexAttribArray(1); // tex
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)0); // pos
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*)12); // tex
    }
    else {
      RX_ERROR(ERR_GL_VAO_VERTEX_NOT_SUPPORTED);
      return false;
    }
    
    unbind();

    return true;
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
