#ifndef ROXLU_SNOTH
#define ROXLU_SNOTH

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

#endif