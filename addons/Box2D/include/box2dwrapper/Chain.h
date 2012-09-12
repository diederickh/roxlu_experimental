#ifndef ROXLU_BOX2D_CHAINH
#define ROXLU_BOX2D_CHAINH

#include <box2dwrapper/Shape.h>
#include <vector>
using std::vector;

namespace roxlu {
namespace box2d {

class Chain : public Shape {
public:
	void add(const float& x, const float& y);
	void make(b2World& w);
	vector<b2Vec2> vertices;
	
};

inline void Chain::add(const float& x, const float& y) {
	vertices.push_back(b2Vec2(PIXELS_TO_METERS(x),PIXELS_TO_METERS(y)));
}

}} // roxlu::box2d

#endif
