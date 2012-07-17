//#include "Tween.h"

#include <roxlu/experimental/Tween.h>

namespace roxlu {
	
Tween::Tween(int type)
	:type(type)
{
}

Tween::~Tween() {
	printf("~Tween\n");
}

void Tween::start(float fromValue, float toValue, rx_uint64 millis, rx_uint64 delayMillis) {
	value = fromValue;
	ready = false;
	delay = delayMillis;
	ends_on = now() + millis + delay;

	
	c = toValue - fromValue;
	b = fromValue;
	d = millis;

	update();
}


void Tween::update() {
	if(ready) {
		return;
	}
	
	rx_uint64 n = now();
	if(n >= ends_on) {
		value = b+c;
		ready = true;
		return;
	}
	
	float t = (d - ((ends_on - n)));
	if(t < 0) {
		return;
	}
	
	switch(type) {
		// Linear
		case TWEEN_LINEAR: {
			value = c * t/d + b;
			return;
		}

		// Qaudratic
		case TWEEN_IN_QUAD: {
			t /= d;
			value = c*t*t + b;
			return;
		}
		
		case TWEEN_OUT_QUAD: {
			t /= d;
			value = -c * t*(t-2) + b;
			return;
		}
		
		case TWEEN_INOUT_QUAD: {
			t /= d/2;
			if (t < 1) {
				value = c/2*t*t + b;
			}
			else {
				t--;
				value = -c/2 * (t*(t-2) - 1) + b;
			}
			return;
		}
		
		// Cubic
		case TWEEN_IN_CUBIC: {
			t /= d;
			value = c*t*t*t + b;
			return;
		}
		
		case TWEEN_OUT_CUBIC: {
			t /= d;
			t--;
			value = c*(t*t*t + 1) + b;
			return;
		}
		
		case TWEEN_INOUT_CUBIC: {
			t /= d/2;
			if (t < 1) {
				value = c/2*t*t*t + b;
			}
			else {
				t -= 2;
				value = c/2*(t*t*t + 2) + b;
			}
			return;
		}
		
		// Quartic
		case TWEEN_IN_QUART: {
			t /= d;
			value = c*t*t*t*t + b;
			return;
		}
		
		case TWEEN_OUT_QUART: {
			t /= d;
			t--;
			value =  -c * (t*t*t*t - 1) + b;
			return;
		}
		
		case TWEEN_INOUT_QUART: {
			t /= d/2;
			if (t < 1) {
				value = c/2*t*t*t*t + b;
			}
			else {
				t -= 2;
				value = -c/2 * (t*t*t*t - 2) + b;
			}
			return;
		}
		
		// Quintic
		case TWEEN_IN_QUINT: {
			t /= d;
			value = c*t*t*t*t*t + b;
			return;
		}
		
		case TWEEN_OUT_QUINT: {
			t /= d;
			t--;
			value = c*(t*t*t*t*t + 1) + b;
			return;
		}
		
		case TWEEN_INOUT_QUINT: {
			t /= d/2;
			if (t < 1) {
				value = c/2*t*t*t*t*t + b;
	
			}
			else {
				t-=2;
				value = c/2*(t*t*t*t*t + 2) + b;
			}
			return;
		}
		
		// Exponential
		case TWEEN_IN_EXPO: {
			value = c * powf(2, 10 * (t/d - 1) ) + b;
			return;
		}
		
		case TWEEN_OUT_EXPO: {
			value = c * ( -powf( 2, -10 * t/d ) + 1 ) + b;
			return;
		}
		
		case TWEEN_INOUT_EXPO: {	
			t /= d/2;
			if (t < 1) {
				value = c/2 * powf( 2, 10 * (t - 1) ) + b;
			}
			else {
				--t;
				value = c/2 * (-powf( 2, -10 * t) + 2 ) + b;
			}
			return;
		}
		
		// Circular
		case TWEEN_IN_CIRC: {
			t /= d;
			value = -c * (sqrt(1 - t*t) - 1) + b;
			return;
		}
		
		case TWEEN_OUT_CIRC: {
			t /= d;
			t--;
			value = c * sqrt(1 - t*t) + b;
			return;
		}
		
		case TWEEN_INOUT_CIRC: {
			t /= d/2;
			if (t < 1) {
				value = -c/2 * (sqrt(1 - t*t) - 1) + b;
			}
			else {
				t -= 2;
				value = c/2 * (sqrt(1 - t*t) + 1) + b;
			}
			return;
		}
		
		// Sinusoidal
		case TWEEN_IN_SINE: {
			value = -c * cosf(t/d * (HALF_PI)) + c + b;
			return;
		}
		
		case TWEEN_OUT_SINE: {
			value = c * sinf(t/d * (HALF_PI)) + b;
			return;
		}
		
		case TWEEN_INOUT_SINE: {
			value = -c/2 * (cosf(PI*t/d) - 1) + b;
			break;
		}
		
		
		// Back
		case TWEEN_IN_BACK: {
			t /= d;
			float s = 1.70158;
			value = c*t*t*((s+1)*t - s) + b;
			return;
		}
		
		case TWEEN_OUT_BACK: {
			float s = 1.70158;
			value = c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
			return;
		}
		
		case TWEEN_INOUT_BACK: {
			float s = 1.70158; 
			if ((t/=d/2) < 1) {
				value = c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
			}
			else {
				value = c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
			}
			return;
		}
		 
		// Elastic
		case TWEEN_IN_ELASTIC: {
			t /= d;
			float p=d*.3f;
			float a=c;
			float s=p/4;
			float post_fix =a*pow(2,10*(t-=1)); 
			value = -(post_fix * sin((t*d-s)*(2*PI)/p )) + b;
			return;
		}
		
		case TWEEN_OUT_ELASTIC: {		
		
			t /= d;
			float p = d*0.3f;
			float a = c;
			float s = p/4;
			value = a*powf(2,-10*t) * sin( (t*d-s)*(2*PI)/p ) + c + b;
		
			return;
		}
		
		case TWEEN_INOUT_ELASTIC: {
			t /= d/2;
			float p=d*(.3f*1.5f);
			float a=c;
			float s=p/4;
			if (t < 1) {
				float pf = a*pow(2,10*(t-=1));
				value = -0.5f*(pf * sin( (t*d-s)*(2*PI)/p )) + b;	
			}
			else {
				t--;
				float pf = a*pow(2,-10*t);
				value = pf * sin( (t*d-s)*(2*PI)/p )*.5f + c + b;
			}
			return;
		}
		
		// Bounce
		case TWEEN_IN_BOUNCE: {
			if ((t/=d) < (1/2.75)) {
				value = c*(7.5625*t*t) + b;
			} 
			else if (t < (2/2.75)) {
				value = c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
			}
			else if (t < (2.5/2.75)) {
				value = c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
			} 
			else {
				value = c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
			}
			
			if(value >= (b+c)) {
				value = (b+c);
			}
			value = (c - value) + b;
			break;
		}
		
		case TWEEN_OUT_BOUNCE: {	
			if ((t/=d) < (1/2.75)) {
				value = c*(7.5625*t*t) + b;
			} 
			else if (t < (2/2.75)) {
				value = c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
			} 
			else if (t < (2.5/2.75)) {
				value = c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
			} 
			else {
				value = c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
			}
			
			if(value >= (b+c)) {
				value = (b+c);
			}

			return;
		}
		
		case TWEEN_INOUT_BOUNCE: {
			printf("TODO TWEEN_INOUT_BOUNCE\n");
			return;
		}
		
		default:break;
	};
}

} // roxlu
