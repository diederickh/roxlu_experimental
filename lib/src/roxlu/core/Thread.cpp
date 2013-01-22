#include <roxlu/core/Thread.h>
#include <stdio.h>

namespace roxlu {
  // ---------------------------------------------------
#if defined(__APPLE__) || defined(__linux__)

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


#if defined(__APPLE__) || defined(__linux__)
  void* Thread::threadFunction(void* arg) {
    Runnable* r = static_cast<Runnable*>(arg);
    r->run();
    return NULL;
  }

  void Thread::create(Runnable& run) {
    pthread_create(&handle, NULL, Thread::threadFunction, &run);
  }

  void Thread::join() {
    pthread_join(handle, NULL);
  }

  void Thread::exit() {
    pthread_exit(NULL);
  }

#elif _WIN32

  DWORD WINAPI Thread::threadFunction(LPVOID arg) {
    Runnable* r = static_cast<Runnable*>(arg);
    r->run();
    return 0;
  }

  void Thread::create(Runnable& run) {
    handle = CreateThread(
                          NULL                       // default security attributes
                          ,0                         // use default stack size                          
                          ,Thread::threadFunction    // thread function name
                          ,&run                      // argument to thread function 
                          ,0                         // use default creation flags 
                          ,&id
                          );
    if(handle == NULL) {
      printf("Thread::Create() - Error creating thread.\n");
    }
  }

  void Thread::join() {
    WaitForSingleObject(handle, INFINITE);
  }

  void Thread::exit() {
    if(handle) {
      CloseHandle(handle);
      handle = NULL;
    }
  }

#endif

} // roxlu
