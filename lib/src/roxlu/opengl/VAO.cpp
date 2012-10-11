//#include "VAO.h"
// @todo make a portable opengl include header which defined base gl function names
#include <roxlu/opengl/VAO.h>
#include <roxlu/core/platform/Platform.h>

namespace roxlu {
#if ROXLU_GL_VARIANT == ROXLU_COCOA
	
	VAO::VAO() 
		:vao_id(-1)
	{
	}

	void VAO::create() {
		glGenVertexArraysAPPLE(1, &vao_id); eglGetError();
	}
	
	void VAO::bind() {											
		if(vao_id == -1) {
			create();
		}
		glBindVertexArrayAPPLE(vao_id); eglGetError();
	}
	
	void VAO::unbind() {
		glBindVertexArrayAPPLE(0); eglGetError();
	}
#elif ROXLU_GL_VARIANT == ROXLU_OPENGLES
	VAO::VAO() 
		:vao_id(-1)
	{
	}

	void VAO::create() {
		glGenVertexArraysOES(1, &vao_id); eglGetError();
	}
	
	void VAO::bind() {											
		if(vao_id == -1) {
			create();
		}
		glBindVertexArrayOES(vao_id); eglGetError();
	}
	
	void VAO::unbind() {
		glBindVertexArrayOES(0); eglGetError();
	}

#elif ROXLU_PLATFORM == ROXLU_WINDOWS
		
	VAO::VAO()
		:vao_id(-1)
	{
	}

	void VAO::create() {
		glGenVertexArrays(1, &vao_id); eglGetError();
	}
	
	void VAO::bind() {
		if(vao_id == -1) {
			create();
		}
		glBindVertexArray(vao_id); eglGetError();
	}
	
	void VAO::unbind() {
		glBindVertexArray(0); eglGetError();
	}
#else
	#error Cannot use VAO  on this operating system
#endif


} // roxlu
