#ifndef RUIH
#define RUIH

#include <vector>


#include "ofMain.h"
//#include "openGL.h"
#include "Shader.h"
#include "VertexData.h"
#include "BitmapFont.h"
#include "Text.h"
#include "VAO.h"
#include "Error.h"

#include "RTypes.h"
#include "RElement.h"
#include "RSlider.h"

using std::vector;
using namespace roxlu;


class Rui {
public:
	Rui();
	~Rui();
	
	void update();
	void draw();
	void debugDraw();
	
	RSlider& addFloat(const string& label, float& value);
	void createOrtho(float w, float h);
	int getHeight();
		
	void onMouseMoved(int x, int y);
	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	
	void setPosition(int x, int y);
	
	
private:
	void onMouseEnter(int x, int y);
	void onMouseLeave(int x, int y);
	void onMouseDragged(int dx, int dy);

	void generateVertices();
	void generatePanelVertices();
	void generateElementVertices(); // create vertices for panel
	void positionElements();
	void flagChanged();
	int x; // TODO: do we create a generic "interactive object" ? 
	int y;
	int w; 
	int h;
	float bg_color[4];	
	int hh;
	int state;
	bool is_changed;
	bool is_mouse_inside;
	bool triggered_drag;
	int mdx; // drag x
	int mdy; // drag y
	int pmx; // prev mouse x
	int pmy; // prev mouse y;
	RuiVertices vd;
	
	bool is_mouse_down;
	static bool shaders_initialized;
	
	static BitmapFont* bmf;
	Text* static_text;
	static Shader gui_shader;
	vector<RElement*> elements;	
	
	float ortho[16];
	float model[16];
	Mat4 model_matrix;
//	Mat4 projection_matrix;
	VAO vao;
	GLuint vbo;
	bool first_run;
};

inline void Rui::flagChanged() {
	is_changed = true;
}

#endif