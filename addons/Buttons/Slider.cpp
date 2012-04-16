#include "Slider.h"

namespace buttons {

Slider::Slider(float& val)
	:value(val)
	,Element()
	,minv(1)
	,maxv(1000)
	,p(0.5)
	,txtval_dx(-1)
	,label_dx(-1)
{
	BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
}

Slider::~Slider() {
}

void Slider::setup() {
	h = 20;
}

void Slider::draw() {
}

void Slider::onMouseEnter(int mx, int my) {
	BSET_COLOR(bg_top_color, 0.0, 0.41, 0.39, 1.0);
	BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
	needsRedraw();
}

void Slider::onMouseUp(int mx, int my) {
	if(!is_mouse_inside) { // mouse up after
		BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		needsRedraw();
	}
}

void Slider::onMouseLeave(int mx, int my) {	
	if(!drag_inside){
		BSET_COLOR(bg_top_color, 0.0, 0.17, 0.21, 1.0);
		BSET_COLOR(bg_bottom_color, 0.0, 0.17, 0.21, 1.0);
		needsRedraw();
	}
}

void Slider::onMouseDragged(int mx, int my, int dx, int dy) {
	if(drag_inside) {
		int local_x = std::min<int>(x+w,std::max<int>(x,mx));
		p = (float)(local_x-x)/w;
		value = minv + (p * (maxv-minv));
		needsRedraw();
		needsTextUpdate();
	//	printf("local_x: %d, x:%d, maxx:%d, perc:%f\n", local_x, x, (x+w), perc);
		
	}
}

void Slider::update() {
}

void Slider::generateStaticText(Text& txt) {
	label_dx = txt.add(x+4, y+2, label, 0.9, 0.9, 0.9, 0.9);
}


void Slider::generateDynamicText(Text& txt) {
	txtval_dx = txt.add(x,y+2, "dynamic",0,0.48,0.98,0.9);
	txt.setTextAlign(txtval_dx, TEXT_ALIGN_RIGHT, x+(w - 5));
};

void Slider::updateTextPosition(Text& staticText, Text& dynamicText) {
	staticText.setTextPosition(label_dx, x+4, y+2);
	updateDynamicText(dynamicText);
}

void Slider::updateDynamicText(Text& txt) {
	txt.setTextPosition(txtval_dx, x, y+2);
	char buf[25];
	sprintf(buf, "%5.3f", value);
	printf("%f, %f\n", value, p);
	txt.updateText(txtval_dx, buf,0,0.48,0.98,0.9);
}

void Slider::generateVertices(ButtonVertices& vd) {
	num_vertices = buttons::createRect(vd, x, y, w, h, bg_top_color, bg_bottom_color);

	BSET_COLOR(bar_filled_color, 0.0, 1.0, 0.0, 0.8);
	BSET_COLOR(bar_empty_color, 0.0, 0.0, 0.0, 0.8);

	int bar_h = 2; // bar height
	int bar_filled_w = w * p;
	int bar_empty_w = w * (1.0 - p);
	int bar_x = x; // start x
	int bar_y = (y + h) - bar_h; // start y

	num_vertices += buttons::createRect(vd, bar_x, bar_y, bar_filled_w, bar_h, bar_filled_color, bar_filled_color);
	num_vertices += buttons::createRect(vd, bar_x + bar_filled_w, bar_y, bar_empty_w, bar_h, bar_empty_color, bar_empty_color);
//	num_vertices = 2 * num_vert; // two rects
//	vd.add(x,y,bg_top_color);
//	vd.add(x+w,y,bg_top_color);
//	vd.add(x+w,y+h,bg_bottom_color);
//	
//	vd.add(x+w,y+h,bg_bottom_color);
//	vd.add(x,y+h,bg_bottom_color);
//	vd.add(x,y,bg_top_color);
	
		
	
}
void Slider::generateVerticesSlider(ButtonVertices& vd) {

//	int bottomh = bh+h; // bottom	
	
//	vd.add(bx, by, bar
	
}


} // namespace buttons