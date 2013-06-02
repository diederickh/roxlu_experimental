#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskOpenSession.h>

CanonTaskOpenSession::CanonTaskOpenSession(Canon* canon)
  :CanonTask(canon, CANON_TASK_OPEN_SESSION)
{
  RX_VERBOSE("@todo OpenSession - Make sure that files are saved to PC, see the sdk + ofxCanon + check capacity on device, make execute an callback when there is no storage left");  
}

CanonTaskOpenSession::~CanonTaskOpenSession() {
  
}

bool CanonTaskOpenSession::execute() {

  if(canon->isLegacy()) {
    RX_ERROR("We did not implement and tested the canon video capture implementation for legacy devices (<30D)");
    ::exit(EXIT_FAILURE);
  }

  if(canon->getState() != Canon::STATE_NONE) {
    RX_ERROR("The device hasn't been opened so we cannot open the session either");
    return false;
  }

  EdsError err = EdsOpenSession(canon->getCameraRef());
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }  
  
  EdsUInt32 save_to = kEdsSaveTo_Host;
  err = EdsSetPropertyData(canon->getCameraRef(), kEdsPropID_SaveTo, 0, sizeof(save_to), &save_to);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }
  
  if(!canon->lockUI()) {
    return false;
  }

  EdsCapacity capacity = {0x7FFFFFFF, 0x1000, 1};
  err = EdsSetCapacity(canon->getCameraRef(), capacity);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }

  if(!canon->unlockUI()) {
    return false;
  }

  canon->setStateOpened();

  return true;
}

