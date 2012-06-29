#ifndef ROXLU_BOX2D_CIRCLEH
#define ROXLU_BOX2D_CIRCLEH

#include "Shape.h"

namespace roxlu {

class Circle : public Shape {
public:
	Circle();
	void make(b2World& w);
	void setRadius(const float& r);
	float radius;
};

inline void Circle::setRadius(const float& r) {
	radius = r;
}

} // namespace roxlu

#endif