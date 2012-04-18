#ifndef ROXLU_SIMULATION2D
#define ROXLU_SIMULATION2D

#include <Box2D/Box2D.h>
#include <vector>
#include "Roxlu.h"
#include "DebugDraw.h"
#include "Shape.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Polygon.h"
#include "Chain.h"
#include "Utils.h"

using std::vector;

namespace roxlu {

class Simulation2D {
public:
	Simulation2D();
	~Simulation2D();
	void update();
	
	Rectangle* createRect(float x0, float x1, float y0, float y1);
	Circle* createCircle(float x, float y, float r);
	Polygon* createPolygon();
	Chain* createChain(); 
	
//	void setPixelsPerMeter(const float& ppm);
	void debugDraw();
	
	float timestep;
	int velocity_iterations;
	int position_iterations;
	b2Vec2 gravity;
	b2World world;
	DebugDraw debug_draw;
	
	vector<Shape*> shapes;
//	float pixels_per_meter;
};
//
//inline void Simulation2D::setPixelsPerMeter(const float& ppm) {
//	pixels_per_meter = ppm;
//}

} // namespace roxlu
#endif