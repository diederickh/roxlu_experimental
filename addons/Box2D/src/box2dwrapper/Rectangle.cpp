#include <box2dwrapper/Rectangle.h>

namespace roxlu {
namespace box2d {

Rectangle::Rectangle() 
	:Shape()
{
}

Rectangle::~Rectangle() {
}

void Rectangle::make(b2World& w) {

	// shape
	b2PolygonShape shape;
	shape.SetAsBox(PIXELS_TO_METERS(hw),PIXELS_TO_METERS(hh));
	
	// body
	if(!body_type_set) {
		if(density == 0.0f) {
			body_def.type = b2_staticBody;
			printf("Creating static rect.\n");
		}
		else {
			body_def.type = b2_dynamicBody;
		}
	}

	body_def.position.Set(PIXELS_TO_METERS(x+hw), PIXELS_TO_METERS(y+hh));
	body = w.CreateBody(&body_def);

	// fixture
	fixture_def.shape = &shape;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	body->CreateFixture(&shape, 0.0f);
	//printf("Create rect with ficture: %f\n", friction);
}

}} // roxlu::box2d

