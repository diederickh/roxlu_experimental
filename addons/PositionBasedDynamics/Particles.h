#ifndef ROXLU_PARTICLESH
#define ROXLU_PARTICLESH

#include "Spring.h"
#include "Particle.h"
#include "BendingConstraint.h"
#include "Vec3.h"
#include <vector>
#include <map>

using namespace roxlu;
using std::vector;
using std::map;

class Particles {
public:
	Particles();
	~Particles();
	
	BendingConstraint* createBendingConstraint(Particle* a, Particle* b, Particle* c, float max);
	BendingConstraint* addBendingConstraint(BendingConstraint* bc);
	
	Particle* createParticle(const float& x, const float& y, const float& z);
	Particle* createParticle(const Vec3& pos, float mass = 1.0f);
	Particle* addParticle(Particle* p);
	
	Spring* createSpring(Particle* a, Particle* b);
	void addSpring(Particle* a, Particle* b);
	void addSpring(Spring* spring);
	bool springExists(Particle* a, Particle* b);
	void removeSpring(Particle* a, Particle* b);
	void addForce(const float& x, const float& y, const float& z);
	void addForce(const Vec3& f);
	void update();
	void draw();
	void repel(float f = 0.9);
	Particle* operator[](const unsigned int& dx);
	size_t size();
	

	vector<Spring*> springs;
	vector<Particle*> particles;
	vector<BendingConstraint*> bending_constraints;
	
	
	map<Particle*, Particle*> springs_ab;
	map<Particle*, Particle*> springs_ba;	
};

inline Particle* Particles::operator[](const unsigned int& dx) {
	return particles[dx];
}

inline size_t Particles::size() {
	return particles.size();
}

inline Particle* Particles::createParticle(const float& x, const float& y, const float& z) {
	return createParticle(Vec3(x,y,z));
}

inline void Particles::addSpring(Particle* a, Particle* b) {
	addSpring(createSpring(a,b));
}

#endif