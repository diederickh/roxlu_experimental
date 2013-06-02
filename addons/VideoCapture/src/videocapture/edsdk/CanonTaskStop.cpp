#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskStop.h>

CanonTaskStop::CanonTaskStop(Canon* canon)
  :CanonTask(canon, CANON_TASK_STOP)
{
}

CanonTaskStop::~CanonTaskStop() {
}

bool CanonTaskStop::execute() {
  return true;
}

