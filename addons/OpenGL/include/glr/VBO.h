#ifndef ROXLU_OPENGL_VBO_H
#define ROXLU_OPENGL_VBO_H

#include <assert.h>
#include <roxlu/core/Log.h>
#include <glr/Vertices.h>
#include <glr/Vertex.h>

#define ERR_GL_VBO_ALREADY_SETUP "The vbo is already setup"
#define ERR_GL_VBO_NOT_SETUP "The vbo wasn't setup properly"
#define ERR_GL_VBO_CANNOT_CREATE "Cannot create the VBO"

namespace gl {

  template<class T>

    class VBO {
  public:
    VBO();
    ~VBO();
    bool setup(GLenum usage);                          /* setup the vbo, usage: GL_STREAM_DRAW, GL_STATIC_DRAW etc.. */
    void update();                                     /* call this whenever the vertices have changed */

    void bind();
    void unbind();

    void push_back(T vertex);
    void clear();
    size_t size();

    T& operator[](size_t dx);                          /* retrieve the vertex stored at dx */

  public:
   Vertices<T> vertices;                               /* the vertices */
   GLuint id;                                          /* vbo id */
   size_t bytes_allocated;                             /* number of bytes allocated on gpu; does not have to be the same size as getNumBytes() as we allocate in chunks */
   GLenum usage;                                       /* hint to the driver; GL_STREAM_DRAW, GL_STATIC_DRAW, etc.. */
  };


  template<class T>
    VBO<T>::VBO() 
    :id(0)
    ,bytes_allocated(0)
    {
    }

  template<class T>
    VBO<T>::~VBO() {

  }

  template<class T>
    bool VBO<T>::setup(GLenum usage) { 

    if(id) {
      RX_ERROR(ERR_GL_VBO_ALREADY_SETUP);
      return false;
    }
    
    this->usage = usage;

    glGenBuffers(1,&id);

    if(!id) {
      RX_ERROR(ERR_GL_VBO_CANNOT_CREATE);
      return false;
    }
    
    return true;
  }

  template<class T>
    void VBO<T>::update() {
    
    if(!id) {
      RX_ERROR(ERR_GL_VBO_NOT_SETUP);
      return;
    }
    
    if(!vertices.hasChanged()) {
      return;
    }
    
    bind();

    if(vertices.mustAllocate(bytes_allocated)) {
      glBufferData(GL_ARRAY_BUFFER, bytes_allocated, NULL, usage);
    }
    
    if(vertices.hasChanged()) {
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.getNumBytes(), vertices.getPtr());
      vertices.unsetHasChanged();
    }

    unbind();
  }

  template<class T>
    void VBO<T>::bind() {

    if(!id) {
      RX_ERROR(ERR_GL_VBO_NOT_SETUP);
      return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, id);
  }

  template<class T>
    void VBO<T>::unbind() {

    if(!id) {
      RX_ERROR(ERR_GL_VBO_NOT_SETUP);
      return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

  }

  template<class T>
    void VBO<T>::push_back(T vertex) {

    vertices.push_back(vertex);
  }

  template<class T>
    inline size_t VBO<T>::size() {

    return vertices.size();
  }

  template<class T>
    inline void VBO<T>::clear() {

    return vertices.clear();
  }

  template<class T>
    inline T& VBO<T>::operator[](size_t dx) {

    assert(dx < size());

    return vertices[dx];
  }

} // gl

#endif
