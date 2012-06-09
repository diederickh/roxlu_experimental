#include "Spring.h"
#include "Particle.h"
#include "Vec3.h"
#include "OpenGL.h"

using namespace roxlu;

Spring::Spring(Particle& a, Particle& b)
	:a(a)
	,b(b)
{
	rest_length = (b.position - a.position).length();
	if(rest_length < 10) {
		rest_length = 10;
	}
	k = 1.0;	
}

void Spring::update(const float& dt) {
	Vec3 dir = b.tmp_position - a.tmp_position;
	const float len = dir.length();
	const float inv_mass = a.inv_mass + b.inv_mass;
	const float f = ((rest_length - len) /inv_mass) * k;
	dir /= len;
	dir *= f;
	dir *= dt;
	
	if(a.enabled) {
		a.tmp_position -= (dir * a.inv_mass);
	}
	if(b.enabled) {
		b.tmp_position += (dir * b.inv_mass);
	}
}

void Spring::draw() {
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_LINES);
		glVertex3fv(a.position.getPtr());
		glVertex3fv(b.position.getPtr());
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}