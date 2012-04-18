#include "Button.h"

namespace buttons {

//Button::Button()
//	:Element(BTYPE_BUTTON)
//	,cb(NULL)
//{
//	h = 20;
//	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
//	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
//	BSET_COLOR(toggle_on_color, 0.0,0.8,0.4,1.0);
//	BSET_COLOR(toggle_off_color, 0.0,0.5,0.2,0.8);
//	BSET_COLOR(button_bg_color, 0.04,0.04,0.04,0.9);
//}
//
//Button::~Button() {
//}

//void Button::generateStaticText(Text& txt) {
//	label_dx = txt.add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
//}

//void Button::updateTextPosition(Text& staticText, Text& dynamicText) {
//	staticText.setTextPosition(label_dx, x+4, y+2);
//}

//void Button::generateVertices(ButtonVertices& vd) {
//	int bt_size = 10;
//	int bt_x = (x+w) - (bt_size+8);
//	int bt_y = y+3;
//	int bt_bg_x = bt_x + 2;
//	int bt_bg_y = bt_y + 2;
//	if(is_mouse_down_inside) {
//		bt_x = bt_bg_x;
//		bt_y = bt_bg_y;
//	}
//	
//	num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
//	num_vertices += buttons::createRect(vd, bt_bg_x, bt_bg_y, bt_size, bt_size, button_bg_color, button_bg_color);
//	num_vertices += buttons::createRect(vd, bt_x, bt_y, bt_size, bt_size, toggle_on_color, toggle_off_color);
//	
//}

//void Button::onMouseDown(int mx, int my) {
//	needsRedraw();
//}
//
//void Button::onMouseUp(int mx, int my) {
//	needsRedraw();
//}

//void Button::onMouseEnter(int mx, int my) {
//	BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
//	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
//	needsRedraw();
//}

//void Button::onMouseLeave(int mx, int my) {
//	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
//	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
//	needsRedraw();
//}

//void Button::onMouseClick(int mx, int my) {
//}



} // namespace buttons