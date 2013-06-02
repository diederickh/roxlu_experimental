#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskEvfDownload.h>

CanonTaskEvfDownload::CanonTaskEvfDownload(Canon* canon)
  :CanonTask(canon, CANON_TASK_EVF_DOWNLOAD)
{
}

CanonTaskEvfDownload::~CanonTaskEvfDownload() {
}

bool CanonTaskEvfDownload::execute() {
  setMustRetry(false);

  EdsError err = EDS_ERR_OK;

  if(!canon->getDevice()->isUsingPCForLiveView()) {
    RX_ERROR("The output device hasn't been set to PC");
    return false;
  }

  EdsEvfImageRef image_ref = NULL;
  EdsUInt32 buffer_size = 2 * 1024 * 1024;
  CanonLiveViewData event_data;

  err = EdsCreateMemoryStream(buffer_size, &event_data.stream_ref);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    EdsRelease(event_data.stream_ref);
    return false;
  }

  err = EdsCreateEvfImageRef(event_data.stream_ref, &image_ref);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    EdsRelease(event_data.stream_ref);
    EdsRelease(image_ref);
    return false;
  }

  err = EdsDownloadEvfImage(canon->getCameraRef(), image_ref);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
  }

  if(err == EDS_ERR_OK) {
    CanonEvent ev(CANON_EVENT_EVF_DATA, (void*)&event_data);
    canon->fireEvent(ev);
  }

  if(event_data.stream_ref != NULL) {
    EdsRelease(event_data.stream_ref);
    event_data.stream_ref = NULL;
  }

  if(image_ref != NULL) {
    EdsRelease(image_ref);
    image_ref = NULL;
  }


  if(err == EDS_ERR_OBJECT_NOTREADY) {
    setMustRetry(true);
  }
  else if(err == EDS_ERR_DEVICE_BUSY) {
    setMustRetry(true);
  }

  return true;
}

