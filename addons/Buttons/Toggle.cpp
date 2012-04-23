#include "Toggle.h"
namespace buttons {

Toggle::Toggle(bool& val, const string& name) 
	:value(val)
	,Element(BTYPE_TOGGLE, name)
{
	h = 20;
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(toggle_on_color, 0.0,0.8,0.4,1.0);
	BSET_COLOR(toggle_off_color, 0.0,0.8,0.2,0.1);
}

Toggle::~Toggle() {
}

void Toggle::generateStaticText(Text& txt) {
	label_dx = txt.add(x+20, y+2, label, 0.9, 0.9, 0.9, 0.9);
}

void Toggle::updateTextPosition(Text& staticText, Text& dynamicText) {	
	staticText.setTextPosition(label_dx, x+20, y+2);
}

void Toggle::generateVertices(ButtonVertices& vd) { 
	num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);
	float* c = toggle_off_color;
	if(value) {
		c = toggle_on_color;
	}
	
	num_vertices += buttons::createRect(vd, x+5, y+4, 10, 10, c, toggle_off_color);
}

void Toggle::onMouseDown(int mx, int my) {
}

void Toggle::onMouseUp(int mx, int my) {
}

void Toggle::onMouseEnter(int mx, int my) {
	BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	needsRedraw();
}

void Toggle::onMouseLeave(int mx, int my) {
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	needsRedraw();
}

void Toggle::onMouseClick(int mx, int my) {
	value = !value;
	needsRedraw();
}


/*
	Toggle(bool& val);
	~Toggle();

	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	

	void setValue(const float& v);

	bool& value;
	
	float bg_top_color[4];
	float bg_bottom_color[4];
	int label_dx;
*/

} // namespace buttons