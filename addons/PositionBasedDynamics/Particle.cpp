#include "Particle.h"
namespace pbd {

Particle::Particle(const Vec3& pos, float mass)
	:position(pos)
	,mass(mass)
	,enabled(true)
	,num_springs(0)
	,size(4.0)
	,energy(1.0)
	,lifespan(40) 
	,age(0.0f)
	,agep(0.0f)
{
	if(mass < 0.0001) {
		mass = 0.0001;
	}
	inv_mass = 1.0f/mass;
	color[0] = color[1] = color[2] = color[3] = 0.8;

}

void Particle::addForce(const Vec3& v) {
	forces += v;
}

void Particle::update(const float& dt) {
}

void Particle::draw() {
	glPointSize(size);
	if(enabled) {
		glColor3fv(color);
	}
	else {
		glColor3f(0.8, 0.0, 0.0);
	}
	
	glBegin(GL_POINTS);
		glVertex3fv(position.getPtr());
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}

} // pbd