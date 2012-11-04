#ifndef ROXLU_TIMERH
#define ROXLU_TIMERH

#include <roxlu/core/platform/Platform.h>

namespace roxlu {

class Timer {	
public: 
	Timer(){
	}
	
	~Timer() {
	}
	
	// returns millis since first call.
	static rx_uint64 millis() {             
		static rx_uint64 t = Timer::now();
		return Timer::now() - t;
	}	
				
	// now in millis
	static rx_uint64 now() {
		#if ROXLU_PLATFORM == ROXLU_APPLE || ROXLU_PLATFORM == ROXLU_IOS || ROXLU_PLATFORM == ROXLU_LINUX
			timeval time;
			gettimeofday(&time, NULL);
			//return (time.tv_sec) * 1000 + (time.tv_usec) / 1000;

			rx_uint64 n = time.tv_usec;
			n /= 1000; // convert seconds to millis
			n += (time.tv_sec * 1000); // convert micros to millis
			return n;

		#elif ROXLU_PLATFORM == ROXLU_WINDOWS 
			return GetTickCount();
			//return timeGetTime(); // not exactly the same value as unix/apple
		#else
			#error Timer::now() not supported 
		#endif
	}
	
	void stop() {
	}
}; // class Timer
}; // roxlu


#endif
