#ifndef ROXLU_VERTEXBUFFERH
#define ROXLU_VERTEXBUFFERH

#include "OpenGL.h"
#include "VertexTypes.h"
#include <vector>

namespace roxlu {
struct VertexBufferEntry {
	VertexBufferEntry(int start, int count, GLenum mode) 
		:start(start)
		,count(count)
		,mode(mode)
	{
	}
	
	int start;
	int count;
	GLenum mode;
};

template<class T>
class VertexBuffer {
public:
	VertexBuffer(size_t bufferPageSize = 4096)
		:buffer_size(0)
		,buffer_page_size(bufferPageSize)
		,needs_resize(true)
		,begin_called(false)
	{
	}
	
	~VertexBuffer() {
	}
	
	void begin(GLenum drawMode) {
		if(begin_called) {
			printf("Already called VertexBuffer<T>::begin!\n");
			exit(0);
		}
		begin_dx = buffer.size();
		begin_called = true;
		begin_mode = drawMode;
	}
	
	// returns entries index
	int end() {
		begin_called = false;
		VertexBufferEntry entry(begin_dx, (buffer.size() - begin_dx), begin_mode);
		entries.push_back(entry);
		return entries.size() - 1;
	}
	
	// returns buffer index
	int add(const typename T::element_type& el) {
		return buffer.add(el);
	}
	
	// returns entries index
	int addRectangle(const float& x, const float& y, const float& w, const float& h) {
		int start = buffer.size();
		int num = buffer.addRectangle(x,y,w,h);
		VertexBufferEntry entry(start, num, GL_TRIANGLES);
		entries.push_back(entry);
		checkResize();
		return entries.size()-1;
	}
	
	bool needsResize() {
		return needs_resize;
	}
	
	void resize() {
		buffer_size += buffer_page_size;
		needs_resize = false;
	}
	
	void clear() {
		buffer.clear();
		entries.clear();
	}
	
	size_t numBytes() {	
		return buffer.numBytes();
	}	
	
	float* getPtr() {
		return buffer.getPtr();
	}
	
	size_t getBufferSize() {
		return buffer_size;
	}
	
	const VertexBufferEntry& operator[](const unsigned int dx) {
		return entries.at(dx);
	}
	
	T buffer;
private:
	void checkResize() {
		if(buffer.numBytes() > buffer_size) {
			needs_resize = true;
		}
	}
	
	GLenum begin_mode;
	bool begin_called;
	int begin_dx;
	bool needs_resize;
	size_t buffer_size;
	size_t buffer_page_size;
	
	std::vector<VertexBufferEntry> entries;
};


typedef VertexBuffer<VerticesPT> VertexBufferPT;

} // namespace roxlu

#endif