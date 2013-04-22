#include <uv/WorkQueue.h>
#include <roxlu/core/Log.h>

void work_queue_worker(uv_work_t* req) {
  WorkQueueReq* wq = static_cast<WorkQueueReq*>(req->data);

  if(!wq->work_queue->isCancelled()) {
    wq->cb_worker(wq->user);
  }
}

void work_queue_ready(uv_work_t* req, int status) {
  WorkQueueReq* wq = static_cast<WorkQueueReq*>(req->data);

  if(!wq->work_queue->isCancelled()) {
    wq->cb_ready(wq->user);
  }

  uv_mutex_lock(&wq->work_queue->mutex);
  wq->work_queue->num_workers--;
  uv_mutex_unlock(&wq->work_queue->mutex);

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

WorkQueue::WorkQueue() 
  :num_workers(0)
  ,is_cancelled(false)
{
  loop = uv_default_loop();
  uv_mutex_init(&mutex);
}

WorkQueue::~WorkQueue() {
  if(count()) {

    is_cancelled = true;

    RX_ERROR(WQ_ERR_STILL_RUNNING);
    while(count()) {
      update();
    }

  }

  num_workers = 0;
  uv_mutex_destroy(&mutex);  
}

void WorkQueue::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

void WorkQueue::addWorker(work_queue_callback workerCB, work_queue_callback readyCB, void* user) {
  uv_mutex_lock(&mutex);
  num_workers++;
  uv_mutex_unlock(&mutex);

  WorkQueueReq* wq = new WorkQueueReq();
  wq->user = user;
  wq->cb_worker = workerCB;
  wq->cb_ready = readyCB;
  wq->work_queue = this;

  uv_work_t* work = new uv_work_t();
  work->data = wq;

  uv_queue_work(loop, work, work_queue_worker, work_queue_ready);
}

size_t WorkQueue::count() {
  size_t result = 0;
  uv_mutex_lock(&mutex);
  result = num_workers;
  uv_mutex_unlock(&mutex);
  return result;
}

