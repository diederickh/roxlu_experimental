#ifndef ROXLU_SPRINGH
#define ROXLU_SPRINGH

class Particle;

class Spring {
public:
	Spring(Particle& a, Particle& b);
	void update(const float& dt);
	void draw();
	
	Particle& a;
	Particle& b;
	float rest_length;
	float k;
};

#endif