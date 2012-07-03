//#include "VAO.h"

#include <roxlu/opengl/VAO.h>

namespace roxlu {

VAO::VAO()  {
	glGenVertexArraysAPPLE(1, &vao_id); eglGetError();
}

VAO& VAO::bind() {
	glBindVertexArrayAPPLE(vao_id); eglGetError();
	return *this;
}

VAO& VAO::unbind() {
	glBindVertexArrayAPPLE(0); eglGetError();
	return *this;
}

} // roxlu
