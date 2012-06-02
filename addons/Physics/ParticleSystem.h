#ifndef ROXLU_PARTICLESYSTEMH
#define ROXLU_PARTICLESYSTEMH

#include "Vec3.h"
#include <vector>

using std::vector;

namespace roxlu {

class Spring;
class Particle;

class ParticleSystem {
public:
	ParticleSystem();
	Particle* createParticle(const float& x, const float& y, const float& z, float mass = 1.0f);
	Particle* createParticle(Vec3 pos, float mass = 1.0f, float friction = 0.96f);
	Spring* createSpring(Particle* a, Particle* b);
	
	void addParticle(Particle& p);
	void addParticle(Particle* p);
	void addForce(const float& x, const float& y, const float& z);
	void addForce(const Vec3& force);
	void update();
	void debugDraw();
	
	vector<Particle*>::iterator begin();
	vector<Particle*>::iterator end();
	
	vector<Particle*> particles;
	vector<Spring*> springs;

}; // ParticleSystem


inline vector<Particle*>::iterator ParticleSystem::begin() {
	return particles.begin();
}

inline vector<Particle*>::iterator ParticleSystem::end() {
	return particles.end();
}


inline void ParticleSystem::addParticle(Particle& p) {
	particles.push_back(&p);
}

inline void ParticleSystem::addParticle(Particle* p) {
	particles.push_back(p);
}

}; // roxlu
#endif