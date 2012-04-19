#include "Rectangle.h"

namespace roxlu {

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
	if(density == 0.0f) {
		body_def.type = b2_staticBody;
	}
	else {
		body_def.type = b2_dynamicBody;
	}

	body_def.position.Set(x+hw, y+hh);
	body = w.CreateBody(&body_def);

	// fixture
	fixture_def.shape = &shape;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	body->CreateFixture(&shape, 0.0f);
}

} // roxlu

