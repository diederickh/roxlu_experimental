#ifndef ROXLU_BOX2D_RECTANGLEH
#define ROXLU_BOX2D_RECTANGLEH

#include <box2dwrapper/Shape.h>

namespace roxlu {
namespace box2d {

class Rectangle : public Shape {
public: 
	Rectangle();
	~Rectangle();
	
	void make(b2World& w);
	void setSize(const float& hw, const float& hh);
	// maybe make this part
	void setPosition(const float& x, const float& y);
	void setTransform(const float& x, const float& y, const float& angle);
	void setRotation(const float angle); 
	float hw;
	float hh;
	float x;
	float y;
};

// top left.
// TODO: use pixels
inline void Rectangle::setPosition(const float& xx, const float& yy) {
	x = xx;
	y = yy;
}

// TODO: use pixels
inline void Rectangle::setSize(const float& halfW, const float& halfH) {
	hw = halfW;
	hh = halfH;
}

// top left.
inline void Rectangle::setTransform(const float& xx, const float& yy, const float& angle) {
	body->SetTransform(b2Vec2(PIXELS_TO_METERS(xx+hw), PIXELS_TO_METERS(yy+hh)), angle);
}

inline void Rectangle::setRotation(const float angle) {
	b2Vec2 pos = body->GetPosition();
	body->SetTransform(pos, angle);
}

}} // roxlu::box2d

#endif
