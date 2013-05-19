#include <roxlu/core/Log.h>
#include <av/AVTypes.h>

AVEncoderSettings::AVEncoderSettings() 
  :in_pixel_format(AV_PIX_FMT_NONE)
  ,in_w(0)
  ,in_h(0)
  ,out_w(0)
  ,out_h(0)
  ,time_base_den(0)
  ,time_base_num(0)
{
}

bool AVEncoderSettings::validate() {
  if(in_w <= 0) {
    RX_ERROR(ERR_AV_INVALID_IN_W);
    return false;
  }

  if(in_h <= 0) {
    RX_ERROR(ERR_AV_INVALID_IN_H);
    return false;
  }

  if(out_w <= 0) {
    RX_ERROR(ERR_AV_INVALID_OUT_W);
    return false;
  }

  if(out_h <= 0) {
    RX_ERROR(ERR_AV_INVALID_OUT_H);
    return false;
  }
  
  if(in_pixel_format == AV_PIX_FMT_NONE) {
    RX_ERROR(ERR_AV_INVALID_IN_FMT);
    return false;
  }


  if(!time_base_den) {
    RX_ERROR(ERR_AV_INVALID_TIMEBASE_DEN);
    return false;
  }

  if(!time_base_num) {
    RX_ERROR(ERR_AV_INVALID_TIMEBASE_NUM);
    return false;
  }

  return true;
}


// ---------------------------------------------------
AVPlayerSettings::AVPlayerSettings() 
  :out_pixel_format(AV_PIX_FMT_NONE)
{
}
