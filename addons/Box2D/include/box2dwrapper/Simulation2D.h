#ifndef ROXLU_SIMULATION2D
#define ROXLU_SIMULATION2D

#include <Box2D/Box2D.h>
#include <vector>
#include <roxlu/Roxlu.h>
#include <box2dwrapper/DebugDraw.h>
#include <box2dwrapper/Shape.h>
#include <box2dwrapper/Circle.h>
#include <box2dwrapper/Rectangle.h>
#include <box2dwrapper/Polygon.h>
#include <box2dwrapper/Chain.h>
#include <box2dwrapper/Utils.h>

using std::vector;

namespace roxlu {
namespace box2d {

class Simulation2D {
public:
	Simulation2D();
	~Simulation2D();
	void update();
	
	Rectangle* createRectangle(float x0, float x1, float y0, float y1);
	Circle* createCircle(float x, float y, float r);
	Polygon* createPolygon();
	Chain* createChain(); 
	
	void destroyShape(Shape* shape);
	
//	void setPixelsPerMeter(const float& ppm);
	void debugDraw(float* viewMatrix, float* projectionMatrix);
	
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

}} // namespace roxlu::box2d
#endif
