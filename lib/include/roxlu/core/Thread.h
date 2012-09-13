#ifndef ROXLU_THREADH
#define ROXLU_THREADH


namespace roxlu {

class Thread;

#ifdef __APPLE__

#include <pthread.h>

struct Mutex {
	Mutex();
	~Mutex();
	void lock();
	void unlock();

	pthread_mutex_t handle;
};
#elif _WIN32

#endif

class Runnable {
public:
	virtual void run() = 0;
};


class Thread {
public:
	Thread();
	~Thread();
	void create(Runnable& run);
#ifdef __APPLE__
	static void* threadFunction(void* arg);
	pthread_t handle;
#elif _WIN32
#endif
};


} // roxlu
#endif
