#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskTakePicture.h>

CanonTaskTakePicture::CanonTaskTakePicture(Canon* canon)
  :CanonTask(canon, CANON_TASK_TAKE_PICTURE)
{
  
}

CanonTaskTakePicture::~CanonTaskTakePicture() {
  
}

bool CanonTaskTakePicture::execute() {
  if(canon->isLegacy()) {
    canon->lockUI();
  }

  takePicture();

  if(canon->isLegacy()) {
    canon->unlockUI();
  }

  return true;
}

bool CanonTaskTakePicture::takePicture() {
  EdsError err = EDS_ERR_OK;
 
  
  err = EdsSendCommand(canon->getCameraRef(), kEdsCameraCommand_TakePicture, 0);

  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }

  return true;
}
