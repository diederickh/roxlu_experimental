#ifndef ROXLU_OPENGL_MESH_H
#define ROXLU_OPENGL_MESH_H

#include <roxlu/opengl/GL.h>
#include <glr/VBO.h>
#include <glr/VAO.h>
#include <glr/Drawer.h>

#define ERR_GL_MS_NOT_BOUND "The mesh is not bound, make sure you bind it before you want to draw."
#define ERR_GL_MS_CANNOT_SETUP_VBO "Cannot setup the VBO for this mesh"
#define ERR_GL_MS_CANNOT_SETUP_VAO "Cannot setup the VAO for this mesh"
#define ERR_GL_MS_CANNOT_SETUP_VAO_ATTRIBS "Cannot setup the VAO attributes for this mesh"
#define ERR_GL_MS_NOT_SETUP "The mesh wasn't setup() yet. Make sure you call setup() for the mesh."

namespace gl {

  template<class T>
  class Mesh {
  public:
    Mesh();
    ~Mesh();
    bool setup(GLenum usage, unsigned int vertexType);            /* setup the mesh, usage = GL_STATIC_DRAW / GL_STREAM_DRAW (a hint to the driver), vertexType = {VERTEX_P, VERTEX_PT, etc.. } */
    void update();                                                /* updates the vbo, if necessary it update the gpu */
    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void push_back(T v);                                          /* add a new vertex to the mesh */
    void clear();
    size_t size();                                                /* returns the number of vertices in the mesh */

    void bind();                                                  /* bind the mesh.. or actually the VAO */
    void unbind();                                                /* unbind the mesh... or actually the VAO */
    
  public:
    VBO<T> vbo;
    VAO vao;
    Mat4 model_matrix;
    bool is_bound;
    bool is_setup;
  }; // Mesh

  template<class T>
    Mesh<T>::Mesh() 
    :is_bound(false)
    ,is_setup(false)
    {
  }

  template<class T>
    Mesh<T>::~Mesh() {
  }

  template<class T>
    bool Mesh<T>::setup(GLenum usage, unsigned int vertexType) {

   if(!vbo.setup(usage)) {
      RX_ERROR(ERR_GL_MS_CANNOT_SETUP_VBO);
      return false;
    }

    if(!vao.setup(vertexType)) {
      RX_ERROR(ERR_GL_MS_CANNOT_SETUP_VAO);
      return false;
    }

    vbo.bind();
 
    if(!vao.enableAttributes()) {
      RX_ERROR(ERR_GL_MS_CANNOT_SETUP_VAO_ATTRIBS);
      vbo.unbind();
      return false;
    }

    vbo.unbind();
    is_setup = true;
    return true;
  }

  template<class T>
    inline void Mesh<T>::update() {

    if(!is_setup) {
      RX_ERROR(ERR_GL_MS_NOT_SETUP);
      return;
    }

    vbo.update();
  }


  template<class T>
    inline void Mesh<T>::push_back(T v) {

    vbo.push_back(v);    
  }

  template<class T>
    inline void Mesh<T>::bind() {

    if(!is_setup) {
      RX_ERROR(ERR_GL_MS_NOT_SETUP);
      return;
    }

    is_bound = true;
    vao.bind();
  }

  template<class T>
    inline void Mesh<T>::unbind() {
    is_bound = false;
    vao.unbind();
  }


  template<class T>
    inline size_t Mesh<T>::size() {

    return vbo.size();
  }

  template<class T>
    inline void Mesh<T>::clear() {

    vbo.clear();
  }

  template<class T>
    inline void Mesh<T>::drawArrays(GLenum mode, GLint first, GLsizei count) {

    assert(glr_context);
    glr_context->drawArrays(*this, mode, first, count);
  }





} // gl
#endif