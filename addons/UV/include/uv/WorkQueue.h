#ifndef ROXLU_WORK_QUEUE_H
#define ROXLU_WORK_QUEUE_H

extern "C" {
#  include <uv.h>
};

typedef void(*work_queue_callback) (void* user);

extern void work_queue_worker(uv_work_t* req);
extern void work_queue_ready(uv_work_t* req, int status);

struct WorkQueueReq {
  WorkQueueReq();
  ~WorkQueueReq();

  work_queue_callback cb_worker;
  work_queue_callback cb_ready;
  void* user;
};

class WorkQueue {
 public:
  WorkQueue();
  ~WorkQueue();
  void addWorker(work_queue_callback workerCB, work_queue_callback readCB, void* user);
  void update();
  uv_loop_t* loop;
};
#endif
