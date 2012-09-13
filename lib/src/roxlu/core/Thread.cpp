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
// CreateMutex, WaitForSingleObject (lock), RelaseMutex, CloseHandle
Mutex::Mutex() {
	::InitializeCriticalSection(&handle);
}
Mutex::~Mutex() {
	::DeleteCriticalSection(&handle);
}
void Mutex::lock() {
	::EnterCriticalSection(&handle);
}
void Mutex::unlock() {
	::LeaveCriticalSection(&handle);
}

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
	return NULL;
}

void Thread::create(Runnable& run) {
	pthread_create(&handle, NULL, Thread::threadFunction, &run);
}

#elif _WIN32

DWORD WINAPI Thread::threadFunction(LPVOID arg) {
	Runnable* r = static_cast<Runnable*>(arg);
	r->run();
	return 0;
}

void Thread::create(Runnable& run) {
	handle = CreateThread(
		NULL
		,0
		,Thread::threadFunction
		,&run
		,0
		,&id
		);
	if(handle == NULL) {
	//	printf("Error creating thread.\n");
	}
		/*
	 CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MyThreadFunction,       // thread function name
            pDataArray[i],          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray[i]);  
			*/
}
#endif

} // roxlu
