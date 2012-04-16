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

enum TextAlign {
 	 TEXT_ALIGN_LEFT
	,TEXT_ALIGN_RIGHT
};

struct TextEntry {
	TextEntry() 
		:start_dx(0)
		,end_dx(0)
		,align(TEXT_ALIGN_LEFT)
		,maxx(0)
	{
		pos[0] = 0;
		pos[1] = 0;
		col[0] = 1;
		col[1] = 1;
		col[2] = 1;
		col[3] = 1;
	}
	
	TextVertices vertices;
	float pos[2];
	float col[4];
	int start_dx;
	int end_dx;
	int align;
	int w; // text width;
	int maxx;
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
	}\
";

const string TEXT_FS =  " \
	uniform sampler2D font_texture; \
	uniform vec4 txtcol; \
	varying vec2 texcoord; \
	void main() { \
		vec4 col = texture2D(font_texture, texcoord); \
		gl_FragColor.rgb = txtcol.rgb;	 \
		gl_FragColor.a = col.r * txtcol.a; \
	} \
";

class Text {
public:
	Text(BitmapFont& bmfont);
	~Text();
	int add(const float& tx, const float& ty, const string& text, float r = 1.0, float g = 1.0, float b = 1.0, float a = 0.9);
	int updateText(const int& textIndex, const string& str,float r = 1.0, float g = 1.0, float b = 1.0, float a = 0.9);
	void setTextPosition(const int& textIndex, const float& x, const float& y);
	void setTextAlign(const int& textIndex, int align, int maxx = 0); // mx is max X when alignment is RIGHT
	void translate(const int& tx, const int& ty);
	void setPosition(const int& tx, const int& ty);
	void updateBuffer();
	void draw();
	void debugDraw();
	void createOrtho(float winW, float winH);
	void setVertexAttributes();
	GLuint vbo;
	VAO vao;
	
	static bool initialized;
	static Shader shader;
	bool is_changed;
	BitmapFont& bmfont;
	vector<TextEntry> texts;
	TextVertices vertices;
	size_t buffer_size;

	
	//Mat4 projection_matrix;
	float model[16];
	float ortho[16];
	int x; 
	int y;
	int win_w;
	int win_h;
};

} // roxlu

#endif