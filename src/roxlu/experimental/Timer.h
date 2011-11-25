#ifndef ROXLU_TIMERH
#define ROXLU_TIMERH

#include "Poco/Timer.h"
#include "Poco/Thread.h"

using Poco::TimerCallback;

namespace roxlu {

template<typename T>
class Timer {	
public: 
	Timer()
		:timer(NULL)
	{
	}
	
	~Timer() {
		stop();
		if(timer != NULL) {
			delete timer;	
		}
	}
	
	void stop() {
		if(timer != NULL) {
			timer->stop();
		}
	}
	
	void onTimer(Poco::Timer& t) {
		callback->onTimer();	
	}
	
	void start(T& callbackObject, long startDelay, long interval) {
		callback = &callbackObject;
		timer = new Poco::Timer(startDelay, interval);
		timer->start(TimerCallback<roxlu::Timer<T> >(*this, &roxlu::Timer<T>::onTimer));
	}
	
	Poco::Timer* timer;
	T* callback;
}; // class Timer
}; // roxlu

#endif