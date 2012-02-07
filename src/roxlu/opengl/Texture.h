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
	void setParams();
	void setPixels(unsigned char* pixels, int width, int height, GLenum format = GL_RGB);
	//void loadImage(string fileName, GLint imageFormat = GL_RGB);
	void bind();
	void unbind();
	GLuint getTextureID();
	string getImageFilePath();
private:
	//ofImage img;
	GLuint texture_id;
	string image_file;
};

}; // roxlu
#endif