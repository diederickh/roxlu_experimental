#ifndef ROXLU_TEXTH
#define ROXLU_TEXTH

#include "ofMain.h"  // tmp
#include "Mat4.h"
#include "OpenGL.h"
#include "Shader.h"
#include "VAO.h"
#include "BitmapFont.h"
#include <vector>

using std::vector;

namespace roxlu {

struct TextEntry {
	TextVertices vertices;
//	Mat4 model_matrix;
	float pos[2];
	int start_dx;
	int end_dx;
};

class Text {
public:
	Text(BitmapFont& bmfont);
	~Text();
	int add(const float& x, const float& y, const string& text);
	int update(const int& textIndex, const float& x, const float& y, const string& str);
	void translate(const int& x, const int& y);
	void setPosition(const int& x, const int& y);
	void updateBuffer();
	void draw();
	void debugDraw();
	GLuint vbo;
	VAO vao;
	
	static bool initialized;
	static Shader shader;
	bool is_changed;
	BitmapFont& bmfont;
	vector<TextEntry> texts;
	TextVertices vertices;
	size_t buffer_size;
	
	Mat4 projection_matrix;
	float model[16];
	float ortho[16];
	int x; 
	int y;
};

} // roxlu

#endif