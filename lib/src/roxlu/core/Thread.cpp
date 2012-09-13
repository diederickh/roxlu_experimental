#include <roxlu/core/Thread.h>

namespace roxlu {
// ---------------------------------------------------
#ifdef __APPLE__

Mutex::Mutex() {
	pthread_mutex_init(&handle, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&handle);
}

void Mutex::lock() {
	pthread_mutex_lock(&handle);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&handle);
}

#elif _WIN32
#endif

// --------------------------------------------------- 
Thread::Thread() {
}

Thread::~Thread() {
}


#ifdef __APPLE__
void* Thread::threadFunction(void* arg) {
   Runnable* r = static_cast<Runnable*>(arg);
	r->run();
}

void Thread::create(Runnable& run) {
	pthread_create(&handle, NULL, Thread::threadFunction, &run);
}

#elif _WIN32
#endif

} // roxlu
