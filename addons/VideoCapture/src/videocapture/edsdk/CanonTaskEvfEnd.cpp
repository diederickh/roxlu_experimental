#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskEvfEnd.h>

CanonTaskEvfEnd::CanonTaskEvfEnd(Canon* canon)
  :CanonTask(canon, CANON_TASK_EVF_END)
{
}

CanonTaskEvfEnd::~CanonTaskEvfEnd() {
}

bool CanonTaskEvfEnd::execute() {
  EdsError err = EDS_ERR_OK;
  EdsUInt32 output_device = 0;
  EdsUInt32 dof_preview = 0;

  output_device = canon->getDevice()->getEvfOutputDevice();
  if( (output_device & kEdsEvfOutputDevice_PC) == 0) {
    return false;
  }

  // Release depth of field in case of depth of field status.
  dof_preview = canon->getDevice()->getEvfDepthOfFieldPreview();
  if(dof_preview != 0) {
    dof_preview = 0;
    err = EdsSetPropertyData(canon->getCameraRef(), kEdsPropID_Evf_DepthOfFieldPreview, 0, sizeof(dof_preview), &dof_preview);
    if(err != EDS_ERR_OK) {
      CANON_ERROR(err);
      return false;
    }
    // needs a tiny delay because new commands are not accepthed 
    rx_sleep_millis(500);
  }

  // Change output device
  output_device &= ~kEdsEvfOutputDevice_PC;
  err = EdsSetPropertyData(canon->getCameraRef(), kEdsPropID_Evf_OutputDevice, 0, sizeof(output_device), &output_device);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    return false;
  }

  canon->setStateOpened();

  return true;
}

