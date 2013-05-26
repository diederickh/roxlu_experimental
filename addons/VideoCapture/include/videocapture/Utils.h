#ifndef ROXLU_VIDEOCAPTURE_UTILS_H
#define ROXLU_VIDEOCAPTURE_UTILS_H

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
}
#include <string>

std::string rx_libav_pixel_format_to_string(enum AVPixelFormat fmt);

#endif
