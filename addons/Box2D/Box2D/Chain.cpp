#include "Chain.h"

namespace roxlu {

void Chain::make(b2World& w) {
	// shape
	b2ChainShape chain;
//	b2Vec2* vert_array = new b2Vec2[vertices.size()];
//	for(int i = 0; i < vertices.size(); ++i) {
//		vert_array[i].Set(vertices[i].x, vertices[i].y);
//	}
//	chain.CreateChain(vert_array, vertices.size());
	chain.CreateChain((b2Vec2*)&vertices[0], vertices.size());

	// body
	if(density == 0.0f) {
		body_def.type = b2_staticBody;
	}
	else {
		body_def.type = b2_dynamicBody;
	}
	
	body = w.CreateBody(&body_def);
	
	// fixture
	fixture_def.shape = &chain;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	
	body->CreateFixture(&fixture_def);
	
	
	/*
	// shape
	b2CircleShape circle;
	circle.m_radius = radius;
	
	
	// body
	if(density == 0.0f) {
		body_def.type = b2_staticBody;
	}
	else {
		body_def.type = b2_dynamicBody;
	}
	
	body = w.CreateBody(&body_def);
	
	
	// fixture
	fixture_def.shape = &circle;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	
	body->CreateFixture(&fixture_def);

	*/

}

}