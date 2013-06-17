/*

  Vertices
  --------

  Basic buffer that can be used for an VBO. We follow the std::vector api, but add 
  a couple of functions that are helpfull when working with (growing) VBOs.

*/

#ifndef ROXLU_VERTICES_H
#define ROXLU_VERTICES_H

#include <assert.h>
#include <vector>

namespace gl {
  template<class T>
    class Vertices {
  public:
    Vertices();
    ~Vertices();

    void assign(int num, T v);
    void push_back(T v);                                       /* add a new vertex to the buffer */
    size_t size();                                             /* number of vertices in buffer */

    void clear();                                              /* remove all vertices from internal buffer and reset the members */ 
    const char* getPtr();                                      /* get a pointer to the data in memory */

    size_t getNumBytes();                                      /* get the number of bytes for all added vertices */
    bool mustAllocate(size_t& bytesToAllocate);                /* use this when you'e working with a VBO. it returns true and sets the bytesToAllocate to the number of bytes needed on the gpu, but clamped at a fixed page size. So the bytesAllocated might be bigger then the number of bytes used. */
    bool hasChanged();                                         /* returns true when we need to update */
    void unsetHasChanged();                                    /* after you've updated the things you need, call this to make super you won't keep updating */

    T& operator[](size_t dx);                                  /* returns a reference tothe T element at location dx */

    /* iterators */
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    typedef typename std::vector<T>::iterator iterator;

  public:
    bool has_changed;                                         /* when you add a new vertex this is set to true; this can be used by a VBO to check if it needs to update */
    std::vector<T> vertices;                                  /* the actuall buffer with vertices */
    size_t bytes_allocated;                                   /* number of bytes you should have allocated on gpu (if using this in combination with a VBO) */
  };


  template<class T>
    Vertices<T>::Vertices() 
    :bytes_allocated(0)
    ,has_changed(false)
    {
    }

  template<class T>
    Vertices<T>::~Vertices() {
    bytes_allocated = 0;
    vertices.clear();
    has_changed = false;
  }
  
  template<class T>
    void Vertices<T>::assign(int num, T v) {
    vertices.assign(num, v);
  }

  template<class T>
    void Vertices<T>::push_back(T v) {

    vertices.push_back(v);

    has_changed = true;
  }

  template<class T>
    size_t Vertices<T>::size() {
    return vertices.size();
  }

  template<class T>
    void Vertices<T>::clear() {
    vertices.clear();
    bytes_allocated = 0;
    has_changed = true;
  }


  template<class T>
    size_t Vertices<T>::getNumBytes() {
    return size() * sizeof(T);
  }

  template<class T>
    bool Vertices<T>::hasChanged() {
    return has_changed;
  }

  template<class T>
    void Vertices<T>::unsetHasChanged() {

    has_changed = false;
  }


  template<class T>
    bool Vertices<T>::mustAllocate(size_t& bytesToAllocate) {
    size_t bytes_needed = getNumBytes();
    bytesToAllocate = 0;
    if(bytes_needed <= bytes_allocated) {
      return false;
    }

    while(bytes_allocated < bytes_needed) {
      bytes_allocated = std::max<size_t>(bytes_allocated * 2, 128);
    }

    bytesToAllocate = bytes_allocated;
    return true;
  }

  template<class T>
    const char* Vertices<T>::getPtr() {
    if(!vertices.size()) {
      return NULL;
    }

    return (const char*)&vertices[0];
  }

  template<class T>
    typename std::vector<T>::iterator Vertices<T>::begin() {
    return vertices.begin();
  }

  template<class T>
    typename std::vector<T>::iterator Vertices<T>::end() {
    return vertices.end();
  }

  template<class T>
    inline T& Vertices<T>::operator[](size_t dx) {
    
    assert(dx < size());
    
    return vertices[dx];
  }

} // gl
#endif
