#include "Texture.h"
#include "Image.h"
#include "Error.h"

namespace roxlu {

Texture::Texture() {
	glGenTextures(1, &texture_id); eglGetError();
}

Texture::~Texture() {
}

void Texture::setParams() {
	bind();
		glEnable(GL_TEXTURE_2D); eglGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); eglGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); eglGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError();
	unbind();
}

void Texture::loadImage(string fileName, GLint imageFormat) {
	// load image.
	image_file = fileName;
	if(!img.loadImage(fileName)) {
		ofLog(OF_LOG_ERROR, "Cannot load file: %s", fileName.c_str());
		return;
	}
	setParams();
	
	// upload texture.
	bind();
		glTexImage2D(
			 GL_TEXTURE_2D, 0
			,GL_RGBA, img.getWidth(), img.getHeight(), 0
			,imageFormat, GL_UNSIGNED_BYTE, img.getPixels());
		eglGetError();
	unbind();
}



void Texture::bind() {
	glEnable(GL_TEXTURE_2D);	
	glBindTexture(GL_TEXTURE_2D, texture_id); 
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D,0); 
}


GLuint Texture::getTextureID() {
	return texture_id;
}

}; // roxlu