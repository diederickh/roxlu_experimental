#ifndef ROXLU_BUTTONSH
#define ROXLU_BUTTONSH

/* 
OpenFrameworks example:
==========================
void testApp::mouseMoved(int x, int y){
	gui.onMouseMoved(x,y);
}
void testApp::mouseDragged(int x, int y, int button){
	cam.onMouseMoved(x,y);
}
void testApp::mousePressed(int x, int y, int button){
	gui.onMouseDown(x,y);	
}
void testApp::mouseReleased(int x, int y, int button){
	gui.onMouseUp(x,y);
}
void testApp::update() {
	gui.update();
}
*/


/** 

Buttons is a simple openGL GUI for tweaking parameters. It's being developed
using openFrameworks but my goal is to make this work in any opengl context on 
both Windows and Mac. 

TODO:
- All elements use absolute coordinates now. Instead of using absolute vertices
we need to check out if it isn't faster to change to a "modelview" approach
where all vertices are translated on GPU. We're using absolute coordinates 
because this makes testing for mouseenter/leave (etc) faster.

*/

#include <vector>

#include "ofMain.h" // @todo remove, needed in c'tor for ofGetWidth()/ofGetHeight()

#include <roxlu/opengl/Shader.h>
#include <roxlu/experimental/BitmapFont.h>
#include <roxlu/experimental/Text.h>
#include <roxlu/opengl/VAO.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/graphics/Color.h>

//#include <buttons/Panel.h>
#include <buttons/Types.h>
#include <buttons/Element.h>
#include <buttons/Slider.h>
#include <buttons/Toggle.h>
#include <buttons/Button.h>
#include <buttons/Radio.h>
#include <buttons/Spline.h>
#include <buttons/Color.h>
#include <buttons/Storage.h>


using std::vector;
using namespace roxlu;

const string BUTTONS_VS = " \
	uniform mat4 projection_matrix; \
	attribute vec4 pos; \
	attribute vec4 col; \
	varying vec4 vcol; \
	void main() { \
		vcol = col; \
		gl_Position = projection_matrix * pos; \
	} \
";


const string BUTTONS_FS = "  \
	varying vec4 vcol; \
	void main() { \
		gl_FragColor = vcol; \
	}\
";


namespace buttons {

struct ButtonsCoords {
	ButtonsCoords()
		:close_x(0)
		,close_y(0)
		,close_w(0)
		,close_h(0)
	{
	}
	
	int close_x;
	int close_y;
	int close_w;
	int close_h;
};

struct ElementByName {
	ElementByName(const string& name)
		:name(name)
	{
	}
	
	bool operator()(const Element* el) {
		return el->name == name;
	}
	string name;
};

class Buttons {
public:
	Buttons(const string& title, int w);
	~Buttons();
	
	void update();
	void draw();
	void debugDraw();
	
	void save();
	void save(const string& file);
	void load();
	void load(const string& file);
	
	Sliderf& addFloat(const string& label, float& value);
	Slideri& addInt(const string& label, int& value);
	Toggle& addBool(const string& label, bool& value);
	ColorPicker& addColor(const string& label, float* value); 
		
	// Spline
	template<class S, class V>
	SplineEditor<S, V>& addSpline(const string& label, S& spline) {
		buttons::SplineEditor<S, V>* el = new SplineEditor<S,V>(label, spline);
		addElement(el, label);
		return *el;
	}
	
	// Radio
	template<class T>
	Radio<T>& addRadio(const string& label, int id, T* cb, const vector<string>& options, int& value) {
		buttons::Radio<T>* el = new Radio<T>(id, options, value, createCleanName(label), cb);
		addElement(el, label);
		
		for(int i = 0; i < options.size(); ++i) {
			Toggle& toggle = addBool(options[i], el->values[i]);
			el->addOption(&toggle);
			toggle.value = false;
			toggle.needsRedraw();
		}
		return *el;
	}
	
	// Button
	template<class T>
	Button<T>& addButton(const std::string& label, int id,  T* cb) {
		buttons::Button<T>* el = new Button<T>(id, cb, createCleanName(label));
		addElement(el, label);
		return *el;
	}
	
	void createOrtho(float w, float h);
	int getElementsHeight();
	int getPanelHeight();
	int getPanelWidth();
		
	// must be called from your app
	void onMouseMoved(int x, int y);
	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onResize(int newW, int newH);
		
	bool isMouseInsidePanel();
	void setPosition(int x, int y);
	void getPosition(int& x, int& y);
	friend class Storage; 
	Element* getElement(const string& name);
	
	void setLock(bool yn);
	void close();
	void open();
	bool isOpen();
	void setColor(const float hue, float a = 1.0);
	string getName();
	
	void addListener(ButtonsListener* listener);
	void notifyListeners(int aboutWhat);
	
private:
	void addElement(Element* el, const string& label);
	void addChildElements(vector<Element*>& children);
	void onMouseEnter(int x, int y);
	void onMouseLeave(int x, int y);
	void onMouseDragged(int dx, int dy);

	void generateVertices();
	void generatePanelVertices();
	void generateElementVertices(); // create vertices for panel
	void updateDynamicTexts();
	void updateStaticTextPositions();
	void positionElements();
	void flagChanged();
	
	string createCleanName(const string& ugly);
	
	vector<Element*> elements;
	vector<ButtonsListener*> listeners;
		
	int x; // TODO: do we create a generic "interactive object" ? 
	int y;
	int w; 
	int h;
	int win_w;
	int win_h;
	float header_color_top[4];	
	float header_color_bottom[4];
	float shadow_color[4];
	int header_h;
	int border;
	int state;
	bool is_changed;
	bool is_mouse_inside_header;
	bool is_mouse_inside_panel;
	bool triggered_drag;
	bool is_open; // mimized or maximized
	int mdx; // drag x
	int mdy; // drag y
	int pmx; // prev mouse x
	int pmy; // prev mouse y;
	string title;
	string name; // name based on title (cleaned)
	int title_dx; // static text index
	ButtonVertices vd;
	int num_panel_vertices; // number of vertices used by the panel itself
	
	bool is_mouse_down;
	static bool shaders_initialized;
	bool is_locked;
	static BitmapFont* bmf;
	Text* static_text;
	Text* dynamic_text;
	static Shader gui_shader;
	size_t allocated_bytes; // number of bytes we allocated for the vbo
	ButtonsCoords coords;
	
	float ortho[16];
	//float model[16];

	VAO vao;
	GLuint vbo;
	bool first_run;
};

inline void Buttons::flagChanged() {
	is_changed = true;
}

inline string Buttons::createCleanName(const string& ugly) {
	string clean_name;
	for(int i = 0; i < ugly.size(); ++i) {
		if(isalnum(ugly[i])) {
			clean_name.push_back(tolower(ugly[i]));
		}
	}
	return clean_name;
}

inline bool Buttons::isMouseInsidePanel() {
	return is_mouse_inside_panel;
}

inline void Buttons::close() {
	is_open = false;
	for(vector<Element*>::iterator it = elements.begin(); it != elements.end(); ++it) {
		Element& el = **it;
		el.hide();
	}
	flagChanged();
}

inline void Buttons::open() {
	is_open = true;
	for(vector<Element*>::iterator it = elements.begin(); it != elements.end(); ++it) {
		Element& el = **it;
		el.show();
	}
	flagChanged();
}

inline string Buttons::getName() {
	return name;
}

} // buttons

#endif
