#include <videocapture/canon/Canon.h>
#include <videocapture/canon/CanonTypes.cpp>

CanonTask::CanonTask(Canon* canon, CanonTask type)
  :canon(canon)
  ,type(type)
{

  if(type == CANON_TASK_NONE) {
    RX_ERROR("Cannot create a task with type CANON_TASK_NONE");
    ::exit(EXIT_FAILURE);
  }

  if(!canon) {
    RX_ERROR("Cannot create a CanonTask with a NULL Canon ptr.");
    ::exit(EXIT_FAILURE);
  }
}


CanonTask::~CanonTask() {
  type = CANON_TASK_NONE;
  canon = NULL;
}
