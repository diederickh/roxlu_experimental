#ifndef ROXLU_PARTICLEH
#define ROXLU_PARTICLEH

#include "OpenGL.h"
#include "Vec3.h"
using namespace roxlu;


namespace pbd {

class Particle {
public:
	Particle(const Vec3& pos, float mass = 1.0f);
	void update(const float& dt);
	void draw();
	void addForce(const Vec3& f);
	
	void disable();
	void enable();
	
	Vec3 forces;	
	Vec3 position;
	Vec3 tmp_position;
	Vec3 velocity;
	
	float mass;
	float size; // used for i.e. drawing
	float energy; // custom use
	float inv_mass;
	bool enabled;
	int num_springs;
	float color[3];
};

inline void Particle::enable() {
	enabled = true;
}

inline void Particle::disable() {
	enabled = false;
}

} // pbd
#endif