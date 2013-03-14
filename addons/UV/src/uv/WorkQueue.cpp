#include <uv/WorkQueue.h>

void work_queue_worker(uv_work_t* req) {
  WorkQueueReq* wq = static_cast<WorkQueueReq*>(req->data);
  wq->cb_worker(wq->user);
}

void work_queue_ready(uv_work_t* req, int status) {
  WorkQueueReq* wq = static_cast<WorkQueueReq*>(req->data);
  wq->cb_ready(wq->user);
  delete wq;
  delete req;
}

// ---------------------------------
WorkQueueReq::WorkQueueReq()
  :cb_worker(NULL)
  ,cb_ready(NULL)
  ,user(NULL)
{
}

WorkQueueReq::~WorkQueueReq() {
  cb_worker = NULL;
  cb_ready = NULL;
  user = NULL;
}

// ---------------------------------

WorkQueue::WorkQueue() {
  loop = uv_default_loop();
}

WorkQueue::~WorkQueue() {
  
}

void WorkQueue::update() {
  uv_run(loop, UV_RUN_ONCE);
}

void WorkQueue::addWorker(work_queue_callback workerCB, work_queue_callback readyCB, void* user) {
  WorkQueueReq* wq = new WorkQueueReq();
  wq->user = user;
  wq->cb_worker = workerCB;
  wq->cb_ready = readyCB;

  uv_work_t* work = new uv_work_t();
  work->data = wq;

  uv_queue_work(loop, work, work_queue_worker, work_queue_ready);
}
