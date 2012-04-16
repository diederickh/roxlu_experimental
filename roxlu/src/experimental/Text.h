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

const string TEXT_VS = "  \
uniform mat4 projection_matrix; \
uniform mat4 model_matrix; \
attribute vec4 pos; \
attribute vec2 tex; \
varying vec2 texcoord; \
void main() { \
	texcoord = tex; \
	gl_Position = projection_matrix * model_matrix * pos; \
}";

const string TEXT_FS =  " \
uniform sampler2D font_texture; \
varying vec2 texcoord; \
void main() { \
	vec4 col = texture2D(font_texture, texcoord); \
	gl_FragColor.rgb = vec3(0.,0.,0.);	 \
	gl_FragColor.a = col.r; \
} \
";

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