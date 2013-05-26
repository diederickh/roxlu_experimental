#ifndef ROXLU_VIDEOCAPTURE_TYPES_H
#define ROXLU_VIDEOCAPTURE_TYPES_H

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
}

#define ERR_VIDCAP_INVALID_WIDTH "Invalid VideoCaptureSettings.width"
#define ERR_VIDCAP_INVALID_HEIGHT "Invalid VideoCaptureSettings.height"
#define ERR_VIDCAP_INVALID_IN_PIX_FMT "Invalid VideoCaptureSettings.in_pixel_format"
#define ERR_VIDCAP_INVALID_FPS "Invalid VideoCaptureSettings.fps"


typedef void(*videocapture_frame_cb)(void* pixels, size_t nbytes, void* user); /* the callback function that gets called by the implementation with video data */

struct AVSize {
  AVSize();
  int width;
  int height;
};

struct AVCapability {
  AVCapability();
  AVSize size;
  AVRational framerate;
  AVPixelFormat pixel_format;
};

struct AVCapabilityFindPixelFormat {
  AVCapabilityFindPixelFormat(enum AVPixelFormat fmt):fmt(fmt){}
  bool operator()(const AVCapability& c) { return c.pixel_format == fmt; } 
  enum AVPixelFormat fmt;
};

struct AVCapabilityFindSize {
  AVCapabilityFindSize(int w, int h):w(w),h(h){}
  bool operator()(const AVCapability& c) { return c.size.width == w && c.size.height == h; } 
  int w;
  int h;
};

// find FPS, 2 decimal significance
struct AVCapabilityFindFrameRate {
  AVCapabilityFindFrameRate(double fps):fps(fps) {}
  bool operator()(const AVCapability& c) { 
    double c_fps = 1.0 / (double(c.framerate.num)/double(c.framerate.den));
    float c_fps_corr = 0;
    char buf[512];
    sprintf(buf, "%2.02f", c_fps);
    sscanf(buf, "%f", &c_fps_corr);
    return c_fps_corr == fps;
  } 
  double fps;
};

struct VideoCaptureSettings {
  VideoCaptureSettings();
  bool validate();                             /* returns true when all member has been correctly set */

  int width;                                   /* the width you want to capture in */ 
  int height;                                  /* the height you want to capture in */
  float fps;                                   /* the framerate you want to captute in,  must  2 digit accurate, e.g. 30.00, 29.97, 20.00 etc... */
  enum AVPixelFormat in_pixel_format;          /* the pixel format you want to receive you're data in.. this must be supported */
  enum AVPixelFormat out_pixel_format;         /* if you set this to something else then AV_PIX_FMT_NONE, we will use SWS to convert incoming data to this format */
};

// Using libavutil/pixfmt.h
enum VideoCaptureFormat {
  VC_FMT_RGB24,   /* packed RGB 8:8:8, 24bpp, RGBRGB.. */
  VC_FMT_YUYV422, /* packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr, mac = kCMPixelFormat_422YpCbCr8_yuvs*/
  VC_FMT_UYVY422, /* packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1, mac = kCMPixelFormat_422YpCbCr8 */ 
  VC_FMT_I420,    /* yuv 4:2:0 */
  VC_NONE
};

// -----------------------------------

inline AVSize::AVSize()
              :width(0)
              ,height(0)
{
}

inline AVCapability::AVCapability() 
                    :pixel_format(AV_PIX_FMT_NONE)
{
}

#endif
