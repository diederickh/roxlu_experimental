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
}

#define ERR_AV_INVALID_IN_FMT "The in_pixel_format is invalid"
#define ERR_AV_INVALID_IN_W "The in_w setting is invalid"
#define ERR_AV_INVALID_IN_H "The in_h setting is invalid"
#define ERR_AV_INVALID_OUT_W "The out_w setting is invalid"
#define ERR_AV_INVALID_OUT_H "The out_h setting is invalid"
#define ERR_AV_INVALID_TIMEBASE_DEN "The time_base_den is invalid"
#define ERR_AV_INVALID_TIMEBASE_NUM "The time_base_num is invalid"

struct AVEncoderSettings {
public:
  AVEncoderSettings();
  bool validate();

public:
  AVPixelFormat in_pixel_format; 
  int in_w;
  int in_h;

  int out_w;
  int out_h;

  double time_base_den;                        /* e.g. 25 (when time_base_num == 1, you'll get 25fps) */
  double time_base_num;                        /* e.g. 1 */
};

#endif
