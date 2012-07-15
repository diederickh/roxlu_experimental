#ifndef ROXLU_BUTTONS_BUTTONH
#define ROXLU_BUTTONS_BUTTONH

/*
#include "Types.h"
#include "Element.h"
*/
#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

class Buttons;

template<class T>
class Button : public Element {
public:
	Button(int id, T* cb, const string& name) 
		:Element(BTYPE_BUTTON, name)
		,id(id)
		,cb(cb)
		,label_dx(0)
	{
		h = 20;
		BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(toggle_on_color, 0.0,0.8,0.4,1.0);
		BSET_COLOR(toggle_off_color, 0.0,0.5,0.2,0.8);
		BSET_COLOR(button_bg_color, 0.04,0.04,0.04,0.9);
	}
	
	~Button() {
	}
	
	void generateStaticText() {
			label_dx = static_text->add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
	}
	void updateTextPosition() {
		static_text->setTextPosition(label_dx, x+4, y+2);
	}
	
	void generateVertices(ButtonVertices& vd) {
		int bt_size = 10;
		int bt_x = (x+w) - (bt_size+8);
		int bt_y = y+3;
		int bt_bg_x = bt_x + 2;
		int bt_bg_y = bt_y + 2;
		if(is_mouse_down_inside) {
			bt_x = bt_bg_x;
			bt_y = bt_bg_y;
		}
		
		buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
		buttons::createRect(vd, bt_bg_x, bt_bg_y, bt_size, bt_size, button_bg_color, button_bg_color);
		buttons::createRect(vd, bt_x, bt_y, bt_size, bt_size, toggle_on_color, toggle_off_color);
	}


	void onMouseDown(int mx, int my) {
		needsRedraw();
	}
	
	void onMouseUp(int mx, int my) {
		needsRedraw();
	}
	
	void onMouseEnter(int mx, int my) {
		BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		needsRedraw();
	}
	
	void onMouseLeave(int mx, int my) {
		BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		needsRedraw();
	}
	
	void onMouseClick(int mx, int my) {
		(*cb)(id);
	}
	
	void save(std::ofstream& ofs) {	}
	void load(std::ifstream& ifs) { }
	bool canSave() { return false; }

	int label_dx;
	float bg_top_color[4];
	float bg_bottom_color[4];
	float toggle_on_color[4];
	float toggle_off_color[4];
	float button_bg_color[4];	
	

	int id;
	T* cb;
};

} // namespace buttons

#endif