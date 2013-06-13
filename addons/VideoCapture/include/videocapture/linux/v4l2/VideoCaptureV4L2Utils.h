#ifndef VIDEOCAPTURE_V4L2_UTILS_H
#define VIDEOCAPTURE_V4L2_UTILS_H

extern "C" {
#  include <libavutil/pixfmt.h>
#  include <linux/videodev2.h>
}

#include <string>

std::string v4l2_pixel_format_to_string(int fmt);
enum AVPixelFormat v4l2_pixel_format_to_libav_pixel_format(int fmt);
int v4l2_libav_pixel_format_to_v4l2_pixel_format(enum AVPixelFormat fmt);
void v4l2_print_format(v4l2_format fmt);

#endif
