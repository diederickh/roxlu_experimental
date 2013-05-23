#ifndef ROXLU_LIBAV_AVTYPES_H
#define ROXLU_LIBAV_AVTYPES_H

#include <string>

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
#  include <libavutil/imgutils.h>
#  include <libavutil/mathematics.h>
#  include <libavcodec/avcodec.h>
#  include <libswscale/swscale.h>
#  include <libavfilter/avfilter.h>
}

#define ERR_AV_INVALID_IN_FMT "The in_pixel_format is invalid"
#define ERR_AV_INVALID_IN_W "The in_w setting is invalid"
#define ERR_AV_INVALID_IN_H "The in_h setting is invalid"
#define ERR_AV_INVALID_OUT_W "The out_w setting is invalid"
#define ERR_AV_INVALID_OUT_H "The out_h setting is invalid"
#define ERR_AV_INVALID_TIMEBASE_DEN "The time_base_den is invalid"
#define ERR_AV_INVALID_TIMEBASE_NUM "The time_base_num is invalid"

#define AV_TYPE_NONE 0
#define AV_TYPE_VIDEO 1
#define AV_TYPE_AUDIO 2

struct AVEncoderSettings {
public:
  AVEncoderSettings();
  bool validate();

public:

  /* audio settings */
  enum AVSampleFormat sample_fmt;              /* the audio sample format e.g. AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16P, AV_SAMPLE_FMT_FLT(P), see: http://libav.org/doxygen/master/samplefmt_8h.html#af9a51ca15301871723577c730b5865c5 */
  int audio_bit_rate;                          /* the bitrate for the audio encoder, e.g. 64000, 128000, 196000 */
  int sample_rate;                             /* e.g. 44100 */
  int num_channels;                            /* audio channels; make sure to select the correct `sample_fmt` for interleaved and/or planar structured audio */

  /* video settings */
  AVPixelFormat in_pixel_format;               /* the input pixel format */
  int in_w;
  int in_h;
  int out_w;
  int out_h;
  double time_base_den;                        /* e.g. 25 (when time_base_num == 1, you'll get 25fps) */
  double time_base_num;                        /* e.g. 1 */
};

struct AVPlayerSettings {                      /* define the behavior of the AVPlayer. e.g. you can use it to convert the decoded frames from the default pixel format to some other format */
  AVPlayerSettings();
  AVPixelFormat out_pixel_format;              /* you can define the output pixel format when decoding. when you don't set this we use the default pixel format in which the video is encoded */
};

#endif
