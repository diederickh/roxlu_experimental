#ifndef ROXLU_OPENGL_MESH_H
#define ROXLU_OPENGL_MESH_H

#include <assert.h>
#include <roxlu/opengl/GL.h>
#include <roxlu/core/Utils.h>
#include <io/OBJ.h>
#include <glr/VBO.h>
#include <glr/VAO.h>
#include <glr/Drawer.h>

#define ERR_GL_MS_NOT_BOUND "The mesh is not bound, make sure you bind it before you want to draw."
#define ERR_GL_MS_CANNOT_SETUP_VBO "Cannot setup the VBO for this mesh"
#define ERR_GL_MS_CANNOT_SETUP_VAO "Cannot setup the VAO for this mesh"
#define ERR_GL_MS_NOT_SETUP "The mesh wasn't setup() yet. Make sure you call setup() for the mesh."
#define ERR_GL_MS_FILETYPE_NOT_SUPPORTED "Cannot load the file because we dont' support the file type yet:`%s` "
#define ERR_GL_MS_CANNOT_LOAD "Error while loading the file"

namespace gl {

  template<class T>
  class Mesh {
  public:
    Mesh();
    ~Mesh();

    bool setup(GLenum usage);                                     /* setup the mesh, usage = GL_STATIC_DRAW / GL_STREAM_DRAW (a hint to the driver) */
    bool load(std::string filename, bool datapath = false);       /* load an 3D mesh from a file */
    void update();                                                /* updates the vbo, if necessary it update the gpu */
    void drawArrays(GLenum mode, GLint first, GLsizei count);
    void push_back(T v);                                          /* add a new vertex to the mesh */
    void clear();
    size_t size();                                                /* returns the number of vertices in the mesh */

    Mat4& mm();                                                   /* returns the model matrix */
    void setPosition(Vec3 pos);                                   /* set the position on the model matrix */
    Vec3 getPosition();                                           /* get the current location of the mesh */
    void setScale(float s);                                       /* sets the scale for x/y/z */

    void bind();                                                  /* bind the mesh.. or actually the VAO */
    void unbind();                                                /* unbind the mesh... or actually the VAO */

    T& operator[](size_t dx);                                     /* retrieve the T element at the given location */
    
  public:
    VBO<T> vbo;
    VAO<T> vao;
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
    bool Mesh<T>::setup(GLenum usage) { 

   if(!vbo.setup(usage)) {
      RX_ERROR(ERR_GL_MS_CANNOT_SETUP_VBO);
      return false;
    }

   if(!vao.setup()) {
      RX_ERROR(ERR_GL_MS_CANNOT_SETUP_VAO);
      return false;
    }
 
   vao.enableAttributes(vbo); 

    vbo.unbind();
    is_setup = true;
    return true;
  }

  template<class T>
    bool Mesh<T>::load(std::string filename, bool datapath) {
    
    std::string ext = rx_get_file_ext(filename);

    if(ext == "obj") {

      OBJ obj;
      if(!obj.load(filename, datapath)) {
        RX_ERROR(ERR_GL_MS_CANNOT_LOAD);
        return false;
      }
      
      if(!obj.copyVertices(vbo)) {
        return false;
      }

    }
    else {
      RX_ERROR(ERR_GL_MS_FILETYPE_NOT_SUPPORTED);
      return false;
    }

    return true;
  }

  template<class T>
    inline void Mesh<T>::update() {

    if(!vbo.size()) {
      return;
    }

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

    assert(vbo.size());
    assert(glr_context);

    glr_context->drawArrays(*this, mode, first, count);
  }

  template<class T>
    inline void Mesh<T>::setPosition(Vec3 position) {

    model_matrix.setPosition(position);
  }

  template<class T>
    inline Vec3 Mesh<T>::getPosition() {

    return model_matrix.getPosition();
  }

  template<class T>
    inline void Mesh<T>::setScale(float s) {

    model_matrix.setScale(s); 
  }

  template<class T>
    inline Mat4& Mesh<T>::mm() {

    return model_matrix;
  }

  template<class T>
    inline T& Mesh<T>::operator[](size_t dx) {

    assert(dx < size());

    return vbo[dx];
  }
} // gl
#endif
