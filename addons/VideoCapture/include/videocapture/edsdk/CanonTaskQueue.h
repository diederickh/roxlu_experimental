#ifndef ROXLU_CANON_TASK_QUEUE_H
#define ROXLU_CANON_TASK_QUEUE_H

extern "C" {
#  include <uv.h>  
}

#include <videocapture/edsdk/CanonTypes.h>
#include <videocapture/edsdk/CanonTaskStop.h>
#include <vector>

#define ERR_CANON_NO_LOOP "No uv_loop created."

// -----------------------------------------------------------

void canon_task_queue_thread(void* queue);

// -----------------------------------------------------------

class Canon;

class CanonTaskQueue {
 public:
  CanonTaskQueue(Canon* canon);
  ~CanonTaskQueue();
  
  bool start();                                             /* user calls this to start the queue thread */
  bool stop();                                              /* user calls this to end the queue thread */
  bool initialize();                                        /* called from the thread function to initialize this object */
  bool shutdown();                                          /* called from the thread function to shutdown the thread/object */

  bool addTask(CanonTask* task);
  void lock();
  void unlock();
  void join();                                              /* join the thread */

 public:
  enum State {
    STATE_NONE,
    STATE_STARTED
  };

  Canon* canon;
  uv_mutex_t mutex;
  uv_thread_t thread_id;
  std::vector<CanonTask*> tasks;
  State state;
};

inline void CanonTaskQueue::lock() {
  uv_mutex_lock(&mutex);
}

inline void CanonTaskQueue::unlock() {
  uv_mutex_unlock(&mutex);
}

inline void CanonTaskQueue::join() {
  uv_thread_join(&thread_id);
}
#endif
