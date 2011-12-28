#ifndef ROXLU_SPRINGH
#define ROXLU_SPRINGH


namespace roxlu {

class Particle;

class Spring {
public:
	Spring(Particle* a, Particle* b, float k = 0.2);
	void debugDraw();
	void update();
	
	float k;
	float rest_length;
	Particle* a;
	Particle* b;
};

}; // roxlu
#endif