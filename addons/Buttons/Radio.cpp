#include "Radio.h"
namespace buttons {
/*
Radio::Radio(const vector<string>& options, int& selected, const string& name) 
	:Element(BTYPE_RADIO, name, BVALUE_NONE)
	,options(options)
{
	h = 20;
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(toggle_on_color, 0.0,0.8,0.4,1.0);
	BSET_COLOR(toggle_off_color, 0.0,0.8,0.2,0.1);
}

Radio::~Radio() {
}

void Radio::generateStaticText(Text& txt) {
	label_dx = txt.add(x+20, y+2, label, 0.9, 0.9, 0.9, 0.9);
}

void Radio::updateTextPosition(Text& staticText, Text& dynamicText) {	
	staticText.setTextPosition(label_dx, x+20, y+2);
}

void Radio::generateVertices(ButtonVertices& vd) { 
	num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
	float* c = toggle_off_color;
	//if(value) {
	//	c = toggle_on_color;
	//}
	
	num_vertices += buttons::createRect(vd, x+5, y+4, 10, 10, c, toggle_off_color);
}

void Radio::onMouseDown(int mx, int my) {
}

void Radio::onMouseUp(int mx, int my) {
}

void Radio::onMouseEnter(int mx, int my) {
	BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	needsRedraw();
}

void Radio::onMouseLeave(int mx, int my) {
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	needsRedraw();
}

void Radio::onMouseClick(int mx, int my) {
	//value = !value;
	needsRedraw();
}
*/

} // namespace buttons