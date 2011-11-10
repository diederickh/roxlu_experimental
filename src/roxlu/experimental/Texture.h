#ifndef ROXLU_TEXTUREH
#define ROXLU_TEXTUREH

#include "ofImage.h"
//#include "OpenGL.h" // can't use this because we use ofImage.h
#include <string>


using std::string;

namespace roxlu {

class Texture {
public:
	Texture();
	~Texture();
	void setParams();
	void loadImage(string fileName, GLint imageFormat = GL_RGB);
	void bind();
	void unbind();
	GLuint getTextureID();

private:
	ofImage img;
	GLuint texture_id;
	string image_file;
};

}; // roxlu
#endif