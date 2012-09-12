#include <box2dwrapper/Circle.h>

namespace roxlu {
namespace box2d {

Circle::Circle() 
	:Shape()
{

}

void Circle::make(b2World& w) {
	// shape
	b2CircleShape circle;
	circle.m_radius = PIXELS_TO_METERS(radius);
	
	
	// body
	if(!body_type_set) {
		if(density == 0.0f) {
			body_def.type = b2_staticBody;
		}
		else {
			body_def.type = b2_dynamicBody;
		}
	}
	body = w.CreateBody(&body_def);
	
	// fixture
	fixture_def.shape = &circle;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	body->CreateFixture(&fixture_def);
//	body->CreateFixture(&circle, 10);

}

}} // roxlu
