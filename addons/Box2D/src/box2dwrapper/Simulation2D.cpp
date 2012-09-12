#include <box2dwrapper/Simulation2D.h>

namespace roxlu {
namespace box2d {

Simulation2D::Simulation2D()
	:gravity(0.0f, 10.0f)
	,world(gravity)
	,velocity_iterations(10)
	,position_iterations(8)
	,timestep(1.0f/30.0f)
//	,pixels_per_meter(100) // 1 meter is 100 pixels
{
	world.SetDebugDraw(&debug_draw);
	
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
//	flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
//	flags += b2Draw::e_centerOfMassBit;
	debug_draw.SetFlags(flags);
}

Simulation2D::~Simulation2D() {
	vector<Shape*>::iterator it = shapes.begin();
	while(it != shapes.end()) {
		delete (*it);
		++it;
	}
}

void Simulation2D::destroyShape(Shape* shape) {
	vector<Shape*>::iterator it = std::find(shapes.begin(), shapes.end(), shape);;
	if(it != shapes.end()) {
		world.DestroyBody(shape->body);
		shapes.erase(it);
		delete shape;
	}
}

void Simulation2D::update() {
	world.Step(timestep, velocity_iterations, position_iterations);
	world.ClearForces();
}

void Simulation2D::debugDraw(float* viewMatrix, float* projectionMatrix) {
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projectionMatrix);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix);
	
	glPushMatrix();
		glScalef(b2_pixels_per_meter, b2_pixels_per_meter, 1);
		world.DrawDebugData();
	glPopMatrix();
}

Rectangle* Simulation2D::createRectangle(float x0, float x1, float y0, float y1) {
	Rectangle* rect = new Rectangle();
	rect->setPosition(x0, y0);
	rect->setSize((x1-x0)*0.5, (y1-y0)*0.5);
	shapes.push_back(rect);
	return rect;
}

Circle* Simulation2D::createCircle(float x, float y, float r) {
	Circle* c = new Circle();
	c->setPosition(x,y);
	c->setRadius(r);
	shapes.push_back(c);
	return c;
}

// ALWAYS USE COUNTER CLOCKWISE VERTICES, MAX 8 VERTICES, SEE maxPolygonVertices
Polygon* Simulation2D::createPolygon() {
	Polygon* p = new Polygon();
	shapes.push_back(p);
	return p;
}

Chain* Simulation2D::createChain() {
	Chain* c = new Chain();
	shapes.push_back(c);
	return c;
}


}} // namespace roxlu::box2d
