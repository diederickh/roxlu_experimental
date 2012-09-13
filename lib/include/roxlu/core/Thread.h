#ifndef ROXLU_THREADH
#define ROXLU_THREADH

#ifdef __APPLE__
#include <pthread.h>
#elif _WIN32
#include <Windows.h>
#endif

namespace roxlu {

class Thread;

#ifdef __APPLE__

struct Mutex {
	Mutex();
	~Mutex();
	void lock();
	void unlock();

	pthread_mutex_t handle;
};
#elif _WIN32


struct Mutex {
	Mutex();
	~Mutex();
	void lock();
	void unlock();
	CRITICAL_SECTION handle; // we're using critical sections; windows has "mutexes" to for IPC
};

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
	static DWORD WINAPI threadFunction(LPVOID param);	
	HANDLE handle;
	DWORD id;
#endif
};


} // roxlu
#endif
