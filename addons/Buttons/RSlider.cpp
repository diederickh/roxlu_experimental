#include "RSlider.h"


RSlider::RSlider(float& val)
	:value(val)
	,RElement()
{
	RSET_COLOR(bg_color, 1.0, 1.0, 0.0, 1.0);
}

RSlider::~RSlider() {
}

void RSlider::setup() {
	h = 20;
}

void RSlider::draw() {
}

void RSlider::onMouseEnter(int x, int y) {
	RSET_COLOR(bg_color, 0.0, 1.0, 0.0, 1.0);
	flagChanged();
}

void RSlider::onMouseLeave(int x, int y) {
	RSET_COLOR(bg_color, 1.0, 0.0, 0.0, 1.0);
	flagChanged();
}

void RSlider::update() {
}

void RSlider::generateStaticText(Text& txt) {
	txt.add(x+4, y+4, label);
}

void RSlider::generateVertices(RuiVertices& vd) {
	vd.add(x,y,bg_color);
	vd.add(x+w,y,bg_color);
	vd.add(x+w,y+h,bg_color);
	
	vd.add(x+w,y+h,bg_color);
	vd.add(x,y+h,bg_color);
	vd.add(x,y,bg_color);
	
	num_vertices = 6;
}