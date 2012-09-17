#ifndef ROXLU_VERTICES_BUFFERH
#define ROXLU_VERTICES_BUFFERH

#include <roxlu/Roxlu.h>

/*

  The VerticesBuffer is a container for growing vertex buffers. It will 
  grow the buffer by multiplying the necessary size by 2.
  
  C = Container for T, e.g. VerticesPT
  T = Vertex type, e.g. VertexPT
  
 */
namespace roxlu {
 	 
template<class C, class T>
class VerticesBuffer {
public:
	VerticesBuffer();
	void add(const T& vertex);
	size_t size();
	const float* getPtr();
	size_t numBytes();
	bool needsToAllocate();
	size_t allocate();
	void clear();
	C& getVertices();
	T& operator[](const unsigned int dx);
private:
	size_t chunk_size;
	size_t bytes_allocated;
	C vertices;
	 
};

template<class C, class T>
VerticesBuffer<C, T>::VerticesBuffer() 
	:bytes_allocated(0)
	,chunk_size(sizeof(T))
{
}

template<class C, class T>
T& VerticesBuffer<C, T>::operator[](const unsigned int dx) {
	return vertices[dx];
}

template<class C, class T>
inline void VerticesBuffer<C, T>::add(const T& vertex) {
	vertices.add(vertex);
}

template<class C, class T>
inline size_t VerticesBuffer<C, T>::size() {
	return vertices.size();
}

template<class C, class T>
inline const float* VerticesBuffer<C, T>::getPtr() {
	return vertices.getPtr();
}

template<class C, class T>
inline C& VerticesBuffer<C, T>::getVertices() {
	return vertices;
}

template<class C, class T>
inline size_t VerticesBuffer<C, T>::numBytes() {
	return vertices.numBytes();
}

template<class C, class T>
inline bool VerticesBuffer<C, T>::needsToAllocate() {
	return (vertices.numBytes() > bytes_allocated);
}

// only clears the internal buffer..
template<class C, class T>
inline void VerticesBuffer<C, T>::clear() {
	vertices.clear();
}

template<class C, class T>
inline size_t VerticesBuffer<C, T>::allocate() {
	size_t needed = vertices.numBytes();
	
	while(bytes_allocated < needed) {
		bytes_allocated = std::max<size_t>(bytes_allocated * 2, chunk_size);
	}
	printf("Exact bytes needed: %zu, returning: %zu, chunk size: %zu\n", needed, bytes_allocated, chunk_size);
	return bytes_allocated;
}

typedef VerticesBuffer<VerticesPT, VertexPT> VerticesBufferPT;

} // roxlu
#endif
