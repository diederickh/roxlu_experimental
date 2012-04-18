#ifndef ROXLU_BOX2D_RECTANGLEH
#define ROXLU_BOX2D_RECTANGLEH

#include "Shape.h"

namespace roxlu {

class Rectangle : public Shape {
public: 
	Rectangle();
	~Rectangle();
	
	void make(b2World& w);
	void setSize(const float& hw, const float& hh);
	void setPosition(const float& x, const float& y);
	float hw;
	float hh;
	float x;
	float y;
};

inline void Rectangle::setPosition(const float& xx, const float& yy) {
	x = xx;
	y = yy;
}

inline void Rectangle::setSize(const float& halfW, const float& halfH) {
	hw = halfW;
	hh = halfH;
}

} // roxlu

#endif