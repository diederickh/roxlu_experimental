#include <algorithm>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonTaskQueue.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>

void canon_task_queue_thread(void* queue) {
  RX_VERBOSE("TASK QueUE TREAD");

  CanonTaskQueue& q = *(static_cast<CanonTaskQueue*>(queue));

  if(!q.initialize()) {
    RX_ERROR("Cannot initalize the queue thread - stopping");
    return;
  }

  std::vector<CanonTask*> work_data;
  bool must_stop = false;
  while(true) {

    q.lock();
    {
      std::copy(q.tasks.begin(), q.tasks.end(), std::back_inserter(work_data));
      q.tasks.clear();
    }
    q.unlock();

    for(std::vector<CanonTask*>::iterator it = work_data.begin(); it != work_data.end(); ++it) {
      CanonTask& task = **it;

      if(task.type == CANON_TASK_STOP) {
        must_stop = true;
        break;
      }
      do {
        task.execute();
      } while(task.mustRetry());

      delete *it;
    } 
    
    work_data.clear();

    if(must_stop) {
      break;
    }

    rx_sleep_millis(1);
  }
  q.shutdown();
}

// -----------------------------------------------------------

CanonTaskQueue::CanonTaskQueue(Canon* canon) 
   :state(STATE_NONE)
   ,canon(canon)
{

  if(uv_mutex_init(&mutex) < 0) {
    RX_ERROR("Cannot initialize the mutex for the queue thread");
    ::exit(EXIT_FAILURE);
  }

}

CanonTaskQueue::~CanonTaskQueue() {
  for(std::vector<CanonTask*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
    delete *it;
  }
  uv_mutex_destroy(&mutex);
}

bool CanonTaskQueue::start() {

  if(state != STATE_NONE) {
    RX_ERROR("Cannot start the task queue because it's already started");
    return false;
  }

  state = STATE_STARTED;

  uv_thread_create(&thread_id, canon_task_queue_thread, this);

  return true;
}

bool CanonTaskQueue::stop() {
  if(state != STATE_STARTED) {
    RX_ERROR("Cannot stop the task queue because it's not started");
    return false;
  }

  RX_VERBOSE("AND STOP!");

  // add the stop task
  CanonTaskStop* task = new CanonTaskStop(canon);
  if(!task) {
    RX_ERROR("Cannot allocate the stop task (!?)");
    return false;
  }
  addTask(task);

  return true;
}

bool CanonTaskQueue::initialize() {
  if(state != STATE_STARTED) {
    RX_ERROR("Cannot initialize because the task queue isn't started yet");
    return false;
  }

  if(!canon->initialize()) {
    RX_ERROR("Cannot initialize the canon instance");
    return false;
  }

  return true;
}

bool CanonTaskQueue::shutdown() {
  if(state != STATE_STARTED) {
    RX_ERROR("Cannot shutdown because the state hasn't been started");
    return false;
  }

  if(!canon->shutdown()) {
    RX_ERROR("Cannot shutdown the canon instance (ignoring this error)");
  }

  state = STATE_NONE;
  return true;
}

bool CanonTaskQueue::addTask(CanonTask* task) {
  lock();
  tasks.push_back(task);
  unlock();
  return true;
}


