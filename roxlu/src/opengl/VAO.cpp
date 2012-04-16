#include "VAO.h"
namespace roxlu {

VAO::VAO() 
	:is_bound(false)
{
	glGenVertexArraysAPPLE(1, &vao_id); eglGetError();
}

VAO& VAO::bind() {
	if(is_bound) {
		return *this;
	}
	
	is_bound = true;
	glBindVertexArrayAPPLE(vao_id); eglGetError();
	return *this;
}

VAO& VAO::unbind() {
	if(!is_bound) {
		return *this;
	}
	glBindVertexArrayAPPLE(0); eglGetError();
	return *this;
}

} // roxlu
