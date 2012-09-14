#ifndef ROXLU_SIMULATION2D
#define ROXLU_SIMULATION2D

/*
Experimental wrapper around box2d
----------------------------------
Rectangle* rect = sim.createRectangle(0, ofGetWidth(), ofGetHeight()-20, ofGetHeight()-10);
rect->make(sim.getWorld());
cam.orthoTopLeft(ofGetWidth(), ofGetHeight(), 0.1f, 10.0f);
cam.setPosition(0.0f, 0.0f, 0.5f);

void testApp::update(){
	sim.update();
}

void testApp::draw(){
	sim.debugDraw(cam.vm().getPtr(), cam.pm().getPtr());
}
*/

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

	b2World& getWorld();
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


inline b2World& Simulation2D::getWorld() {
	return world;
}

}} // namespace roxlu::box2d
#endif
