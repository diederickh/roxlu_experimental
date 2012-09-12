#include <box2dwrapper/Chain.h>

namespace roxlu {
namespace box2d {

void Chain::make(b2World& w) {
	// shape
	b2ChainShape chain;
	chain.CreateChain((b2Vec2*)&vertices[0], vertices.size());

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
	fixture_def.shape = &chain;
	fixture_def.density = density;
	fixture_def.friction = friction;
	fixture_def.restitution = restitution;
	
	body->CreateFixture(&fixture_def);
}

}}
