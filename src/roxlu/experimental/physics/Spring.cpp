#include "Spring.h"
#include "Particle.h"

namespace roxlu {

Spring::Spring(Particle* a, Particle* b, float k)
	:a(a)
	,b(b)
	,k(k)
{
	rest_length = (b->getPosition() - a->getPosition()).length();
}


void Spring::update() {
	Vec3 dir = b->getPosition() - a->getPosition();
	float curr_length = dir.length();
//	float dist = dir.length();
//	float f = (rest_length - dist) / dist ;
//	dir /= dist;
//	dir *= f;
//	a->addForce(-dir * k);
//	b->addForce(dir * k);

	float f = ( curr_length - rest_length) * k;
	dir.normalize();
	dir *= f;
	a->addForce(dir);
	b->addForce(-dir);


//	float f = ( curr_length - rest_length) ;
//	dir /= curr_length;
//	dir *= f;
//	dir *= k;
//	a->addForce(dir);
//	b->addForce(-dir);
//	if(a->is_enabled) {
//		a->position += dir;
//	}
//	if(b->is_enabled) {
//		b->position -= dir;
//	}
	

//	float len = ( curr_length - rest_length) ;
//	float f = len / rest_length;
//	
//	a->addForce(f * dir);
//	b->addForce(f * -dir);
	
//	dir.print();
//	dir *= f;

//	if(a->is_enabled) {
//		//a->position += dir;
//		a->velocity += dir;
//	}
//	if(b->is_enabled) {
//		b->velocity -= dir;
//	}
//	
}

}; // roxlu
