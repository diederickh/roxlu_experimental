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
  ,sample_fmt(AV_SAMPLE_FMT_S16P)
  ,audio_bit_rate(0)
  ,sample_rate(0)
  ,num_channels(0)
  ,use_audio(false)
  ,audio_codec(AV_CODEC_ID_MP3)
{
}

bool AVEncoderSettings::validate() {
  return validateAudio() && validateVideo();
}

bool AVEncoderSettings::validateVideo() {
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

bool AVEncoderSettings::validateAudio() {
  if(!useAudio()) {
    return true;
  }

  if(audio_codec != AV_CODEC_ID_MP3) {
    RX_ERROR(ERR_AV_INVALID_AUDIO_CODEC_ID);
    return false;
  }

  if(!sample_rate) {
    RX_ERROR(ERR_AV_INVALID_SAMPLE_RATE);
    return false;
  }
  
  if(num_channels != 1) {
    RX_ERROR(ERR_AV_INVALID_NUM_CHANNELS);
    return false;
  }

  if(!audio_bit_rate) {
    RX_ERROR(ERR_AV_INVALID_AUDIO_BIT_RATE);
    return false;
  }

  return true;
}


// ---------------------------------------------------
AVPlayerSettings::AVPlayerSettings() 
  :out_pixel_format(AV_PIX_FMT_NONE)
{
}
