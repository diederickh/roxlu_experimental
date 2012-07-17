#ifndef ROXLU_RELEMENTH
#define ROXLU_RELEMENTH

#include <roxlu/experimental/Text.h>
#include <roxlu/graphics/Color.h>
#include <buttons/Types.h>
#include <string>
#include <fstream>

using namespace roxlu;
using std::string;

namespace buttons {

class Buttons;

class Element {
public:	
	Element(int type, const string& name);
	~Element();

	virtual void setup(){}
	virtual void update(){}
	virtual void draw(Shader& shader, const float* pm){} // custom drawing! pm = projection matrix
	
	virtual void generateStaticText() {}
	virtual void updateTextPosition() {}
	virtual void generateDynamicText() {};
	virtual void updateDynamicText() {};
	virtual void generateVertices(ButtonVertices& shapeVertices) = 0;
	
	virtual void getChildElements(vector<Element*>& elements) {}
	virtual void positionChildren(){}
				
	virtual void onMouseMoved(int mx, int my) { } // global  move
	virtual void onMouseDragged(int mx, int my, int dx, int dy) { } // global drag (check: drag_inside)
	virtual void onMouseDown(int mx, int my) { } // global down
	virtual void onMouseUp(int mx, int my) { } // global up
	virtual void onMouseClick(int mx, int my) { } // when click and released inside
	virtual void onMouseEnter(int mx, int my) { }
	virtual void onMouseLeave(int mx, int my) { }
	
	virtual bool canSave() = 0;
	virtual void save(std::ofstream& ofs) = 0; 	// each element which stores something must write a size_t with the amount of data it stores
	virtual void load(std::ifstream& ifs) = 0;
	
	virtual void onSaved(){}  // gets called once all data has been saved
	virtual void onLoaded(){}  // gets called once all data has been loaded 
	
	virtual void hide();
	virtual void show();
		
	virtual Element& setColor(const float hue, const float sat, const float bright, float a = 1.0f);
	virtual Element& setColor(const float* col, int num = 3);
	
 	void needsRedraw();
	void needsTextUpdate(); // when you want to change the dynamic text
	
	int x;
	int y; 
	int w;
	int h;
	int type;

	string label;
	string name;
	
	int state;
	bool is_child; // an element (like the color picker) can have children. children needs to be positioned by the parent!
	bool is_mouse_inside;
	bool is_mouse_down_inside;
	bool is_visible; 
	bool needs_redraw;	
	bool needs_text_update;
	bool drag_inside; // did the drag started from inside the element.
	//int num_vertices; // we don't need this anymore
	
	float* bg_top_color;
	float* bg_bottom_color;
	float col_text[4];
	float col_bg_default[4];
	float col_bg_top_hover[4];
	float col_bg_bottom_hover[4];
	
	Text* static_text;
	Text* dynamic_text;

};

inline void Element::needsRedraw() {
	needs_redraw = true;
}

inline void Element::needsTextUpdate() {
	needs_text_update = true;
}

inline Element& Element::setColor(const float* col, int num) {
	if(num = 3) {
		setColor(col[0], col[1], col[2]);
	}
	else if(num == 4) {
		setColor(col[0], col[1], col[2], col[3]);
	}	
	return *this;	
}
	
inline Element& Element::setColor(const float hue, const float sat, const float bright, float a) {
	// set default color
//	float rr = std::max<float>(0.1,r);
//	float gg = std::max<float>(0.1,g);
//	float bb = std::max<float>(0.1,b);
//	printf("%f, %f, %f\n", rr,gg,bb);
//	rr = r;
//	gg = g; 
//	bb = b;
//	printf("%f, %f, %f\n", rr,gg,bb);	
//	float hue,sat,bright;
//	RGB_to_HSL(rr,gg,bb,&hue, &sat, &bright);
	float ss, ll;
	ss = std::min<float>(sat, 0.5);
	ll = std::min<float>(bright, 0.3);
	
	float rr,gg,bb;
	HSL_to_RGB(hue, ss, ll, &rr, &gg, &bb);
	BSET_COLOR(col_bg_default, rr, gg, bb, a);
	bg_bottom_color = col_bg_default;
	bg_top_color = col_bg_default;
	col_bg_top_hover[3] = a;
	col_bg_bottom_hover[3] = a;
	col_text[3] = 0.9f;	
	
	float top_bright = std::min<float>(1.0, ll + 0.1);
	float bot_bright = std::max<float>(0.0, ll - 0.1);

	HSL_to_RGB(hue, ss , top_bright, &col_bg_top_hover[0], &col_bg_top_hover[1], &col_bg_top_hover[2]);
	HSL_to_RGB(hue, ss , bot_bright, &col_bg_bottom_hover[0], &col_bg_bottom_hover[1], &col_bg_bottom_hover[2]);
	HSL_to_RGB(hue, ss , ll + 0.6, &col_text[0], &col_text[1], &col_text[2]);
	
//	HSL_to_RGB(hue, sat * 1.5, bright * 1.5, &col_bg_top_hover[0], &col_bg_top_hover[1], &col_bg_top_hover[2]);
//	HSL_to_RGB(hue, sat * 1.2, bright * 1.2, &col_bg_bottom_hover[0], &col_bg_bottom_hover[1], &col_bg_bottom_hover[2]);
//	HSL_to_RGB(hue, sat + 0.6, bright + 0.6, &col_text[0], &col_text[1], &col_text[2]);
	//printf("---> Col bg default: %f, %f, %f\n", col_bg_top_hover[0], col_bg_top_hover[1], col_bg_top_hover[2]);
//	col_bg_top_hover[0] = 0.229032;
//	col_bg_top_hover[1] = 0.389355;
//	col_bg_top_hover[2] = 0.480968;

	return *this;
}

inline void Element::show() {
	is_visible = true;
}

inline void Element::hide() {
	is_visible = false;
}


} // namespace buttons

#endif