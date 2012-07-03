//#include "PBO.h"

#include <roxlu/opengl/PBO.h>

PBO::PBO(GLuint nTarget)
:target(nTarget)
,buffer_size(0)
,color_type(GL_RGBA)
{
	glGenBuffers(1, &pbo_id);eglGetError();
	glGenTextures(1, &texture_id);eglGetError();
}

// simple support, only for rgba images
PBO& PBO::setup(int nWidth, int nHeight, GLenum nColorType) {
	width = nWidth;
	height = nHeight;
	color_type = nColorType;
	buffer_size = width * height * 4;
	unbind(); // see note in header.
	glBindTexture(GL_TEXTURE_2D, texture_id); eglGetError();
	glEnable(GL_TEXTURE_2D); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError();
	glTexImage2D(
		 GL_TEXTURE_2D, 0
		,GL_RGBA, width, height, 0
        ,color_type, GL_UNSIGNED_BYTE, NULL
	);eglGetError();
	//unbind();
	return *this;
}

PBO& PBO::setPixels(unsigned char* pRGBA) {
	if(buffer_size == 0) {
		printf("PBO: buffer size is zero. Did you call setup()?");
		return *this;
	}
	
	// Pass the pRGBA pixel data to the buffer object.
	// ------------------------------------------------
	// reset the contents of the buffer.
	bind();
	glBufferData(target, buffer_size, NULL, GL_STREAM_DRAW); eglGetError();

	// map openGL memory to our application.
	void* pbo_memory = NULL;
	pbo_memory = glMapBuffer(target, GL_WRITE_ONLY); eglGetError();

    // modify the buffer data (all data);
	memcpy(pbo_memory, pRGBA, buffer_size); 
	
	// and unmap the image buffer.
	glUnmapBuffer(target);eglGetError();
	
	
	// Now update the texture.
	// -----------------------
	glBindTexture(GL_TEXTURE_2D, texture_id); eglGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0,
		0,0, width, height
		,color_type, GL_UNSIGNED_BYTE, (char*)0
	); eglGetError();
	//unbind();
}

PBO& PBO::bind() {
	glBindTexture(GL_TEXTURE_2D, texture_id);eglGetError();
	glBindBuffer(target, pbo_id); eglGetError();
	
	return *this;
}

PBO& PBO::unbind() {

	glBindBuffer(target,0); eglGetError();
	return *this;
}


