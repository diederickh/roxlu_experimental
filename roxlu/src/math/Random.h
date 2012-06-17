#ifndef ROXLU_RANDOMH
#define ROXLU_RANDOMH

// random: source openFrameworks + stackoverflow
#include "Vec3.h"
#include "float.h"
#include <sys/time.h>

static void initRandom() {
	struct timeval tv;
	gettimeofday(&tv, 0);
	long int n = (tv.tv_sec ^ tv.tv_usec) ^ getpid();
	srand(n);
}

static void initRandom(int val) {
	srand((long) val);
}

static float random(float max) {
	return max * rand() / (RAND_MAX + 1.0f);
}

static float random(float x, float y) {
	float high = 0;
	float low = 0;
	float result = 0;
	
	high = std::max<float>(x,y);
	low = std::min<float>(x,y);
	result = low + ((high-low) * rand()/(RAND_MAX + 1.0));
	return result;
}

static float randomf() {
	float rand_num = 0;
	rand_num = (rand()/(RAND_MAX + 1.0)) * 2.0 - 1.0;
	return rand_num;
}

static float randomuf() {
	float rand_num = 0;
	rand_num = rand()/(RAND_MAX + 1.0);
	return rand_num;
}


static roxlu::Vec3 randomVec3() {
	roxlu::Vec3 r;
	float phi = random(3.1415f * 2.0f);
	float costheta = random(-1.0f, 1.0f);

	float rho = sqrt(1.0f - costheta * costheta);
	r.x = rho * cos(phi);
	r.y = rho * sin(phi);
	r.z = costheta;
	return r;
}
#endif