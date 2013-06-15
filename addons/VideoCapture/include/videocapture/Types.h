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

// The VideoCapture add supports multiple capture implementations, these are used together
// with the VideoCapture() constructor where you tell what capture implementation you want
// to use. For each platform we select a default implementation
enum VideoCaptureImplementation {
  VIDEOCAPTURE_DIRECTSHOW,                   // Windows - DEFAULT - Use the DirectShow samplegrabber
  VIDEOCAPTURE_WINDOWS_MEDIA_FOUNDATION,     // Windows -         - Use the Windows Media Foundation grabber
  VIDEOCAPTURE_AVFOUNDATION,                 // Mac     - DEFAULT - Use the AVFoundation grabber
  VIDEOCAPTURE_V4L2,                         // Linux   - DEFAULT - Use the Video4Linux sample grabber
  VIDEOCAPTURE_EDSDK
};

// AVSize represents the capture width/height a device can use to capture (most devices support multiple sizes)
struct AVSize {
  AVSize();
  int width;
  int height;

  bool operator<(const AVSize& o) const {
    return width < o.width && height < o.height;
  }

  bool operator==(const AVSize& o) const {
    return width == o.width && height == o.height;
  }

};

// AVCapability represent the capabilities that a capture device supports. 
struct AVCapability {
  AVCapability();
  int index;                                       /* default to -1, but can be used by the implementation to reference an internal index */
  AVSize size;
  AVRational framerate;
  AVPixelFormat pixel_format;
};

// Used to filter capabilities on pixel formats
struct AVCapabilityFindPixelFormat {
  AVCapabilityFindPixelFormat(enum AVPixelFormat fmt):fmt(fmt){}
  bool operator()(const AVCapability& c) { return c.pixel_format == fmt; } 
  enum AVPixelFormat fmt;
};

// Used to filter capabilites on size
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

// The VideoCaptureSettings are used to open a capture device
struct VideoCaptureSettings {
  VideoCaptureSettings();
  bool validate();                             /* returns true when all member has been correctly set */

  int width;                                   /* the width you want to capture in */ 
  int height;                                  /* the height you want to capture in */
  float fps;                                   /* the framerate you want to capture in, must  2 digit accurate, e.g. 30.00, 29.97, 20.00 etc... */
  enum AVPixelFormat in_pixel_format;          /* the pixel format you want to receive you're data in.. this must be supported */
  enum AVPixelFormat out_pixel_format;         /* if you set this to something else then AV_PIX_FMT_NONE, we will use SWS to convert incoming data to this format */
};

// -----------------------------------

inline AVSize::AVSize()
              :width(0)
              ,height(0)
{
}

inline AVCapability::AVCapability() 
                    :pixel_format(AV_PIX_FMT_NONE)
                   ,index(-1)
{
}

#endif
