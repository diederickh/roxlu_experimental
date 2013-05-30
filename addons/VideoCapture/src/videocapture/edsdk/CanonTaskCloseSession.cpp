#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskCloseSession.h>

CanonTaskCloseSession::CanonTaskCloseSession(Canon* canon)
  :CanonTask(canon, CANON_TASK_CLOSE_SESSION)
{
  
}

CanonTaskCloseSession::~CanonTaskCloseSession() {
  
}

bool CanonTaskCloseSession::execute() {

  if(canon->getState() != Canon::STATE_OPENED) {
    RX_ERROR("Cannot close the session because the device hasn't been opened yet.");
    return false;
  }

  if(canon->getCameraRef() == NULL) {
    RX_ERROR("Cannot close session because we can't find a camera ref");
    return false;
  }

  EdsError err = EdsCloseSession(canon->getCameraRef());
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }
  return true;
}

