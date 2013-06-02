#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskEvfStart.h>

CanonTaskEvfStart::CanonTaskEvfStart(Canon* canon)
  :CanonTask(canon, CANON_TASK_EVF_START)
{
}

CanonTaskEvfStart::~CanonTaskEvfStart() {
}

bool CanonTaskEvfStart::execute() {
  EdsError err = EDS_ERR_OK;

  if(canon->getDevice()->getEvfMode() == 0) {
    EdsUInt32 evf_mode = 1;
    err = EdsSetPropertyData(canon->getCameraRef(), kEdsPropID_Evf_Mode, 0, sizeof(evf_mode), &evf_mode);
    if(err != EDS_ERR_OK) {
      CANON_ERROR(err);
      return false;
    }
  }

  EdsUInt32 output_device = canon->getDevice()->getEvfOutputDevice();
  output_device |= kEdsEvfOutputDevice_PC;
  err = EdsSetPropertyData(canon->getCameraRef(), kEdsPropID_Evf_OutputDevice, 0, sizeof(output_device), &output_device);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }

  canon->setStateLiveView();

  return true;
}

