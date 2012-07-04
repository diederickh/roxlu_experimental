//#include "VAO.h"

#include <roxlu/opengl/VAO.h>
#include <roxlu/core/platform/Platform.h>

namespace roxlu {
#if ROXLU_PLATFORM == ROXLU_APPLE
	
	VAO::VAO()  {
		//printf("APPLE!!\n");	
	}

	void VAO::create() {
		glGenVertexArraysAPPLE(1, &vao_id); eglGetError();
		//glGenVertexArrays(1, &vao_id);// eglGetError();
	}
	
	void VAO::bind() {
		glBindVertexArrayAPPLE(vao_id); eglGetError();
	}
	
	void VAO::unbind() {
		glBindVertexArrayAPPLE(0); eglGetError();
	}

#elif ROXLU_PLATFORM == ROXLU_WINDOWS
		
	VAO::VAO()  {
		//printf("VAO WINDOWS!\n");
	}

	void VAO::create() {
		glGenVertexArrays(1, &vao_id);// eglGetError();
	}
	
	void VAO::bind() {
		//printf("binding VAO with id: %d\n", vao_id);
		glBindVertexArray(vao_id); eglGetError();
	}
	
	void VAO::unbind() {
		glBindVertexArray(0); eglGetError();
	}
#else
	#error Cannot use VAO  on this operating system
#endif


} // roxlu
