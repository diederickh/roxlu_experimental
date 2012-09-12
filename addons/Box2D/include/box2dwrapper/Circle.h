#ifndef ROXLU_BOX2D_CIRCLEH
#define ROXLU_BOX2D_CIRCLEH

#include <box2dwrapper/Shape.h>

namespace roxlu {
namespace box2d {

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

}} // namespace roxlu::box2d

#endif
