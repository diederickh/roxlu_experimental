#include <videocapture/Types.h>
#include <roxlu/core/Log.h>

VideoCaptureSettings::VideoCaptureSettings() 
  :out_pixel_format(AV_PIX_FMT_NONE)
  ,in_pixel_format(AV_PIX_FMT_NONE)
  ,width(0)
  ,height(0)
  ,fps(0.0f)
{
}

bool VideoCaptureSettings::validate() {
  bool result = true;

  if(in_pixel_format == AV_PIX_FMT_NONE) {
    RX_ERROR(ERR_VIDCAP_INVALID_IN_PIX_FMT);
    result = false;
  }

  if(width == 0) {
    RX_ERROR(ERR_VIDCAP_INVALID_WIDTH);
    result = false;
  }

  if(height == 0) {
    RX_ERROR(ERR_VIDCAP_INVALID_HEIGHT);
    result = false;
  }

  if(fps == 0.0f) {
    RX_ERROR(ERR_VIDCAP_INVALID_FPS);
    result = false;
  }

  return result;
      
}
