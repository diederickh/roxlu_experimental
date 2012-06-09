#ifndef ROXLU_PARTICLEH
#define ROXLU_PARTICLEH

#include "Vec3.h"

namespace roxlu {

class Particle {
public:
	Particle();
	Particle(Vec3 position);
	Particle(Vec3 position, float mass, float fric = 0.96f);
	virtual void addForce(const Vec3& f);
	virtual void update();
	virtual void debugDraw();

	void enable();
	void disable();
	bool isEnabled();
	
	void setMass(float m);
	void setPosition(Vec3 p);
	void setVelocity(Vec3 v);
	
	float getMass();
	float getInverseMass();
	
	Vec3 getPosition();
	Vec3 getVelocity();
	Vec3 getForces();
	
	void setLifeTime(const unsigned int& ltime);
	int getAge();

	
	Vec3 position;
	Vec3 velocity;
	Vec3 forces;
	float friction;
	float mass;
	float inverse_mass;
	bool is_enabled;
	unsigned int lifetime;
	int age;
}; // Particle


// -----------------------------------------------------------------------------
inline void Particle::setLifeTime(const unsigned int& ltime) {
	lifetime = ltime;
}

inline int Particle::getAge() {
	return age;
}


inline float Particle::getMass() {
	return mass;
}

inline float Particle::getInverseMass() {
	return inverse_mass;
}

inline Vec3 Particle::getPosition() {
	return position;
}

inline Vec3 Particle::getVelocity() {
	return velocity;
}

inline Vec3 Particle::getForces() {
	return forces;
}

inline bool Particle::isEnabled() {
	return is_enabled;
}

inline void Particle::setMass(float m) {
	if(m < 0.001) {
		mass = 0.001;
	}
	else {
		mass = m;
	}
	inverse_mass = 1.0f/mass;
}

inline void Particle::setPosition(Vec3 p) {
	position = p;
}

inline void Particle::setVelocity(Vec3 v) {
	velocity = v;
}

inline void Particle::enable() {
	is_enabled = true;
}

inline void Particle::disable() {
	is_enabled = false;
}

}; // roxlu
#endif