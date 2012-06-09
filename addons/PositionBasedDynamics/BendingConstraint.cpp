#include "BendingConstraint.h"
#include "Particle.h"

BendingConstraint::BendingConstraint(
	Particle& a
	,Particle& b
	,Particle& c
	,float max
)
	:a(a)
	,b(b)
	,c(c)
	,max(max)
{
	k = 0.9;
}

void BendingConstraint::update(const float& dt) {
	Vec3 ba = a.tmp_position - b.tmp_position;
	Vec3 bc = c.tmp_position - b.tmp_position;
	ba.normalize();
	bc.normalize();
	float angle = acos(dot(ba,bc));
	//if(angle > max) {
		Vec3 ca = c.tmp_position - a.tmp_position;
		ca.normalize();
		float f = (max - angle) * k;
		c.tmp_position += (ca * f * dt * c.inv_mass);
	//	printf("YES! %f\n", angle * (180/3.13));
	//}
}
	