#ifndef ROXLU_PARTICLEH
#define ROXLU_PARTICLEH

#include "OpenGL.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>

using std::vector;
using namespace roxlu;


namespace pbd {

class Spring;

class Particle {
public:
	Particle(const Vec3& pos, float mass = 1.0f);
	void update(const float& dt);
	void draw();
	void addForce(const float& x, const float& y, const float& z);
	void addForce(const Vec3& f);
	
	void disable();
	void enable();
	
	void setColor(const float& r, const float& g, const float& b, float a = 1.0f);
	void setPosition(const float* p);
	
	void addSpring(Spring* sp);
	
	Vec3 forces;	
	Vec3 position;
	Vec3 tmp_position;
	Vec3 velocity;
	Vec2 tex; // can be used to store texture coordinates
	
	float mass;
	float size; // used for i.e. drawing
	float energy; // custom use; used to as repel force
	float inv_mass;
	bool enabled;
	int num_springs;
	int dx; // index of particle in Particles container.
	float color[4];
	
	float lifespan;
	float age;
	float agep;
	
	vector<Spring*> springs; // connected springs;
	
};

inline void Particle::enable() {
	enabled = true;
}

inline void Particle::disable() {
	enabled = false;
}

inline void Particle::addSpring(Spring* sp) {
	springs.push_back(sp);
}


inline void Particle::addForce(const float& x, const float& y, const float& z) {
	forces.x += x;
	forces.y += y;
	forces.z += z;
}

inline void Particle::setColor(const float& r, const float& g, const float& b, float a) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

inline void Particle::setPosition(const float* p) {
	position.x = p[0];
	position.y = p[1];
	position.z = p[2];
}

} // pbd
#endif