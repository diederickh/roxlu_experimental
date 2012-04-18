#ifndef ROXLU_BOX2D_POLYGONH
#define ROXLU_BOX2D_POLYGONH

#include "Shape.h"
#include <vector>
using std::vector;

// TODO: implement

namespace roxlu {

class Polygon : public Shape {
public:
	void add(const float& x, const float& y);
	void make(b2World& w);
	
	vector<b2Vec2> vertices;
};

} // roxlu
#endif