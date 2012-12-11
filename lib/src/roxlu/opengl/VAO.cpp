#ifdef ROXLU_GL_WRAPPER

#include <roxlu/opengl/VAO.h>
#include <roxlu/core/platform/Platform.h>

namespace roxlu {
#if ROXLU_PLATFORM == ROXLU_APPLE
	
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
#elif ROXLU_PLATFORM == ROXLU_IOS
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

#elif ROXLU_PLATFORM == ROXLU_WINDOWS || ROXLU_PLATFORM == ROXLU_LINUX
		
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

#endif // ROXLU_GL_WRAPPER
