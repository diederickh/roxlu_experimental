#ifndef ROXLU_RANDOMH
#define ROXLU_RANDOMH

// random: source openFrameworks + stackoverflow

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

#endif