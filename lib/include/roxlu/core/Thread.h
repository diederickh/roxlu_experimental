#ifndef ROXLU_THREADH
#define ROXLU_THREADH

#if defined(__APPLE__) || defined(__linux__)
#include <pthread.h>
#elif _WIN32
#include <Windows.h>
#endif

namespace roxlu {

  class Thread;

#if defined(__APPLE__) || defined(__linux__)

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
    void join();
    void exit();
#if defined(__APPLE__) || defined(__linux__)
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
