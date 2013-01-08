#ifndef POTJEPISSEN_IMAGEH
#define POTJEPISSEN_IMAGEH

#include "OpenGL.h"
#include "Error.h"
#include "Texture.h"
#include "Mat4.h"
#include "File.h"
#include "WebP.h"
#include "ShaderTypes.h"
#include "VertexTypes.h"
#include <string>
using namespace roxlu;

class rxImage {
public:
	rxImage(Shader& shader);
	~rxImage();
	bool load(const std::string& filename);
	void setPixels(unsigned char* pixels, const unsigned int& w, const unsigned int& h, GLenum format);
	void setPosition(const float& x, const float& y);
	void draw();
	void draw(const float& x, const float& y);
	void draw(const float& x, const float& y, const float& w, const float& h);
	int getWidth();
	int getHeight();
private:
	Mat4 model_matrix;
	Shader& shader;
	GLuint vbo;
	GLuint vao;
	Texture tex;
	bool buffer_created;
	int prev_w;
	int prev_h;
	int width;
	int height;
};


inline int rxImage::getWidth() {
	return width;
}

inline int rxImage::getHeight() {
	return height;
}
#endif