#ifndef ROXLU_SPRINGH
#define ROXLU_SPRINGH

namespace pbd {

class Particle;

class Spring {
public:
	Spring(Particle& a, Particle& b);
	void update(const float& dt);
	void draw();
	
	void enable();
	void disable();
	bool isEnabled();
	
	Particle& a;
	Particle& b;
	float rest_length;
	float curr_length;
	float k;
	bool enabled;
};


inline void Spring::enable() {
	enabled = true;
}

inline void Spring::disable() {
	enabled = false;
}

inline bool Spring::isEnabled() {
	return enabled;
}

}
#endif