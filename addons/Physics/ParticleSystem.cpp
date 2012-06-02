#include "OpenGL.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Spring.h"

namespace roxlu {

ParticleSystem::ParticleSystem() {
}

Particle* ParticleSystem::createParticle(const float& x, const float& y, const float& z, float mass) {
	Particle* p = new Particle(Vec3(x,y,z), mass);
	particles.push_back(p);
	return p;
}

Particle* ParticleSystem::createParticle(Vec3 pos, float mass, float friction) {
	Particle* p = new Particle(pos, mass, friction);
	particles.push_back(p);
	return p;
}

Spring* ParticleSystem::createSpring(Particle* a, Particle* b) {
	Spring* s = new Spring(a,b);
	springs.push_back(s);
	return s;
}


void ParticleSystem::update() {
		
	const int num_springs = springs.size();
	Spring** spr_ptr = (num_springs != 0) ? &springs.front() : NULL;
	for(int i = 0; i < num_springs; ++i) {	
		spr_ptr[i]->update();
	}

	const int num = particles.size();
	Particle** ptr = (num != 0) ? &particles.front() : NULL;
	for(int i = 0; i < num; ++i) {
		ptr[i]->update();
	}

}

void ParticleSystem::addForce(const float& x, const float& y, const float& z) {
	addForce(Vec3(x,y,z));
}

void ParticleSystem::addForce(const Vec3& f)  {
	const int num = particles.size();
	Particle** ptr = (num != 0) ? &particles.front() : NULL;
	for(int i = 0; i < num; ++i) {
		ptr[i]->addForce(f);
	}
}

void ParticleSystem::debugDraw() {
	const int num_springs = springs.size();
	Spring** spr_ptr = (num_springs != 0) ? &springs.front() : NULL;
	glColor3f(0.7f,0.7f,0.7f);
	glColor3f(1,0,0);
//	printf("%d<--\n", num_springs);
	glBegin(GL_LINES);
	for(int i = 0; i < num_springs; ++i) {	
//		spr_ptr[i]->a->position.print();
//		spr_ptr[i]->b->position.print();
		//printf("--\n");
		glVertex3fv(&(spr_ptr[i]->a->position.x));
		glVertex3fv(&(spr_ptr[i]->b->position.x));
	}
	glEnd();
	
	const int num = particles.size();
	Particle** ptr = (num != 0) ? &particles.front() : NULL;
	glPointSize(4.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	for(int i = 0; i < num; ++i) {
		glVertex3fv(&(ptr[i]->position.x));
	}
	glEnd();
	
	
}

}; // roxlu