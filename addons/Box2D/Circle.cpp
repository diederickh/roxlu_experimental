#include "Circle.h"

namespace roxlu {

Circle::Circle() 
	:Shape()
{

}

void Circle::make(b2World& w) {
	// shape
	b2CircleShape circle;
	circle.m_radius = PIXELS_TO_METERS(radius);
	
	
	// body
	if(density == 0.0f) {
		body_def.type = b2_staticBody;
	}
	else {
		body_def.type = b2_dynamicBody;
	}
	body_def.angularDamping = 0.01;
	body = w.CreateBody(&body_def);
	
	
	// fixture
	fixture_def.shape = &circle;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
//	printf("Circle.friction:%f\n", friction);
//	printf("Circle.density: %f\n", density);
//	printf("\n");
	body->CreateFixture(&fixture_def);
	
	/*
	b2PolygonShape rect;
	float hw = (x1-x0) * 0.5;
	float hh = (y1-y0) * 0.5;
	rect.SetAsBox(hw,hh);
	
	// body
	b2BodyDef bdef;
	bdef.position.Set(x0+hw, y0+hh);
	b2Body* body = world.CreateBody(&bdef);

	// fixture
	body->CreateFixture(&rect, 0.0f);

	*/
}

} // roxlu