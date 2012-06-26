#ifndef ROXLU_TEXTUREH
#define ROXLU_TEXTUREH

#include <string>
#include "OpenGL.h"

using std::string;

namespace roxlu {

class Texture {
public:
	Texture();
	~Texture();
	void setWrapS(GLenum wrapS);
	void setWrapT(GLenum wrapT);
	void setWrap(GLenum wrapST);
	void setWrap(GLenum wrapS, GLenum wrapT);
	void setMinFilter(GLenum minFilter);
	void setMagFilter(GLenum magFilter);
	void setInternalFormat(GLint internalFormat);
	void setParams();
	void setPixels(const unsigned char* pixels, int width, int height, GLenum format = GL_RGB);

	void bind();
	void unbind();
	GLuint getID();
	string getImageFilePath();
	
private:
	GLenum wrap_s;
	GLenum wrap_t;
	GLenum min_filter;
	GLenum mag_filter;
	GLint internal_format;

	GLuint texture_id;
	string image_file;
};

inline void Texture::setInternalFormat(GLint internalFormat) {
	internal_format = internalFormat;
}

inline void Texture::setWrapS(GLenum wrapS) {
	wrap_s = wrapS;
}

inline void Texture::setWrapT(GLenum wrapT) {
	wrap_t = wrapT;
}


inline void Texture::setWrap(GLenum wrapST) {
	setWrap(wrapST, wrapST);
}

inline void Texture::setWrap(GLenum s, GLenum t) {
	wrap_s = s;
	wrap_t = t;
}

inline void Texture::setMinFilter(GLenum minFilter) {
	min_filter = minFilter;
}

inline void Texture::setMagFilter(GLenum magFilter) {
	mag_filter = magFilter;
}

}; // roxlu
#endif