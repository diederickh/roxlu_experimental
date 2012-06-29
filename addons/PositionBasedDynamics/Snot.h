#ifndef ROXLU_SNOTH
#define ROXLU_SNOTH


namespace pbd {

class Particles;
class Particle;
class Spring;

class Snot {
public: 
	Snot(Particles& ps);
	void update();
		
	float connect_dist;
	float break_dist;
	Particles& ps;
};

} // pbd
#endif