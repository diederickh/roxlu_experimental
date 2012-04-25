#ifndef APOLLO_TWEENH
#define APOLLO_TWEENH

// Based on: http://robertpenner.com/easing/penner_easing_as2.zip

#include <sys/time.h>

#ifndef PI
	#define PI 3.1415926535897932384626433832795
#endif

#ifndef HALF_PI
	#define HALF_PI 1.5707963267948966192313216916398
#endif

#ifndef TWO_PI
	#define TWO_PI 6.283185307179586476925286766559
#endif

enum TweenFuncs {
	 TWEEN_LINEAR
	
	,TWEEN_IN_QUAD
	,TWEEN_OUT_QUAD
	,TWEEN_INOUT_QUAD
	
	,TWEEN_IN_CUBIC
	,TWEEN_OUT_CUBIC
	,TWEEN_INOUT_CUBIC
	
	,TWEEN_IN_QUART
	,TWEEN_OUT_QUART
	,TWEEN_INOUT_QUART
	
	,TWEEN_IN_QUINT
	,TWEEN_OUT_QUINT
	,TWEEN_INOUT_QUINT
	
	,TWEEN_IN_EXPO
	,TWEEN_OUT_EXPO
	,TWEEN_INOUT_EXPO
	
	,TWEEN_IN_CIRC
	,TWEEN_OUT_CIRC
	,TWEEN_INOUT_CIRC
	
	,TWEEN_IN_SINE
	,TWEEN_OUT_SINE
	,TWEEN_INOUT_SINE
	
	,TWEEN_IN_BACK
	,TWEEN_OUT_BACK
	,TWEEN_INOUT_BACK
	
	,TWEEN_IN_ELASTIC
	,TWEEN_OUT_ELASTIC
	,TWEEN_INOUT_ELASTIC
	
	,TWEEN_IN_BOUNCE
	,TWEEN_OUT_BOUNCE
	,TWEEN_INOUT_BOUNCE
	
};

class Tween {
public:
	Tween(int type = TWEEN_LINEAR);
	~Tween();
	
	Tween& setType(int t);
	uint64_t now();
	void start(float fromValue, float toValue, uint64_t millis, uint64_t delay = 0);
	void update();
	
	operator float() {
		return value;
	}
		
	Tween& operator()(int t = TWEEN_LINEAR) {
		type = t;
		return *this;
	}
	
	float c;
	float d;
	float b;
	float value;

	uint64_t delay;
	uint64_t t;
	uint64_t ends_on;
	
	int type;
	bool ready;
	float* result;
};

inline Tween& Tween::setType(int t) {
	type = t;
	return *this;
}

inline uint64_t Tween::now() {
	timeval time;
	gettimeofday(&time, NULL);
	uint64_t n = time.tv_usec;
	n /= 1000;
	n += (time.tv_sec * 1000);
	return n;
}


#endif