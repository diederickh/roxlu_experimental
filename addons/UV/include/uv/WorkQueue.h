#ifndef ROXLU_WORK_QUEUE_H
#define ROXLU_WORK_QUEUE_H

extern "C" {
#  include <uv.h>
};

typedef void(*work_queue_callback) (void* user); 

extern void work_queue_worker(uv_work_t* req);
extern void work_queue_ready(uv_work_t* req, int status);

#define WQ_ERR_STILL_RUNNING "There are still workers running.. waiting for them to finish"

class WorkQueue;

struct WorkQueueReq {
  WorkQueueReq();
  ~WorkQueueReq();

  work_queue_callback cb_worker;
  work_queue_callback cb_ready;
  void* user;
  WorkQueue* work_queue;
};

class WorkQueue {
 public:
  WorkQueue();
  ~WorkQueue();
  void addWorker(work_queue_callback workerCB, work_queue_callback readyCB, void* user);
  void update();
  size_t count();
  bool isCancelled();

  size_t num_workers;
  uv_loop_t* loop;
  uv_mutex_t mutex;

 private:
  bool is_cancelled;
};

inline bool WorkQueue::isCancelled() {
  return is_cancelled;
}

#endif
