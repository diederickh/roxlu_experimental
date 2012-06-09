#include "Particle.h"
#include "OpenGL.h"

namespace roxlu {
Particle::Particle() 
	:position(0,0,0)
	,mass(1.0)
	,inverse_mass(1.0)
	,is_enabled(true)
	,age(0)
	,lifetime(0)
{
}

Particle::Particle(Vec3 pos) 
	:position(pos)
	,mass(1.0)
	,inverse_mass(1.0)
	,is_enabled(true)
	,age(0)
	,lifetime(0)
{

}


Particle::Particle(Vec3 pos, float m, float fric) 
	:position(pos)
	,mass(m)
	,friction(fric)
	,is_enabled(true)
	,age(0)
	,lifetime(0)
{
	if(mass < 0.001) {
		mass = 0.001;
	}
	inverse_mass = 1.0f/mass;
}

void Particle::addForce(const Vec3& f) {
	forces += f;
}

void Particle::update() {
	age++;
	if(!is_enabled) {
		return;
	}
	
	
	if(lifetime != 0 && age >= lifetime) {
		is_enabled = false;	
	}
	
	forces *= inverse_mass;
	velocity += forces;
	position += velocity;
	forces = 0.0f;
	velocity *= friction;
}

void Particle::debugDraw() {
	if(!is_enabled) {
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glBegin(GL_POINTS);
		glVertex3fv(&position.x);
	glEnd();
}
	

}; // roxlu