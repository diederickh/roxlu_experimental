#include "Particle.h"

Particle::Particle(const Vec3& pos, float mass)
	:position(pos)
	,mass(mass)
	,enabled(true)
	,num_springs(0)
{
	if(mass < 0.0001) {
		mass = 0.0001;
	}
	inv_mass = 1.0f/mass;
}

void Particle::addForce(const Vec3& v) {
	forces += v;
}

void Particle::update(const float& dt) {
}

void Particle::draw() {
	glPointSize(4.0);
	if(enabled) {
		glColor3f(0.0, 0.8, 0.0);
	}
	else {
		glColor3f(0.8, 0.0, 0.0);
	}
	
	glBegin(GL_POINTS);
		glVertex3fv(position.getPtr());
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}