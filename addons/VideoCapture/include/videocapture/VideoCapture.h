#ifndef ROXLU_VIDEOCAPTURE_H
#define ROXLU_VIDEOCAPTURE_H

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <string.h> 
#include <vector>
#include <roxlu/core/Log.h>
#include <videocapture/Types.h>
#include <videocapture/VideoCaptureBase.h>

#if defined(__APPLE__)
#  include <videocapture/mac/VideoCaptureMac.h>
#elif defined(_WIN32)
#  include <videocapture/win/directshow/VideoCaptureDirectShow.h>
#  include <videocapture/win/mediafoundation/VideoCaptureMediaFoundation.h>
#elif defined(__linux)
#  include <videocapture/linux/v4l2/VideoCaptureV4L2.h>
#endif

#if defined(ROXLU_USE_EDSDK) // @todo make this in option in the cmake files (edsdk is only 32bit)
#  include <videocapture/edsdk/Canon.h>
#endif

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
#  include <libavcodec/avcodec.h>
#  include <libswscale/swscale.h>
}

#define ERR_VIDCAP_SWS "Cannot create an sws context that we need to convert between pixel formats"
#define ERR_VIDCAP_ALLOC_FRAME "Cannot allocate a frame"
#define ERR_VIDCAP_ALLOC_FRAMEBUF "Error while allocating the AVFrame that we use to convert webcam pixel format"
#define ERR_VIDCAP_SWS_SCALE "Something went wrong while trying to scale the input data "
#define ERR_VIDCAP_FILL_PIC "Error while trying to fill the AVFRame; fill returned 0"
#define ERR_VIDCAP_NOT_OPENED "Cannot close the device because it's not yet openend"

// ---------------------------------------------------------

typedef void (*videocapture_frame_callback)(AVFrame* pixelsIn, size_t nbytesIn, AVFrame* pixelsOut, size_t nbytesOut, void* user);                 /* this function will be calls when we've processed a frame pixelsIn are the raw bytes; pixelsOut are the pixels converted to pixels_out_format (if set) */
void videocapture_process_frame_callback(void* pixels, size_t nbytes, void* user);                                                                 /* will be called by the specific capture implementation (like DirectShow, WindowsMediaFoundation, OSX AVFoundation etc.. */

// ---------------------------------------------------------

class VideoCapture {
 public:

#if defined(_WIN32)
  VideoCapture(VideoCaptureImplementation imp = VIDEOCAPTURE_DIRECTSHOW);
#elif defined(__APPLE__)
  VideoCapture(VideoCaptureImplementation imp = VIDEOCAPTURE_AVFOUNDATION);
#elif defined(__linux)  
  VideoCapture(VideoCaptureImplementation imp = VIDEOCAPTURE_V4L2);
#else
  VideoCapture(VideoCaptureImplementation imp);
#endif

  ~VideoCapture();

  /* Capture control */
  int listDevices();                                                                                                                               /* get a list of supported devices */
  bool openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB, void* user);                                          /* open the given device, and pass the callback that should receive the video frames af AVFrame* */
  bool closeDevice();                                                                                                                              /* close the device (and stop capturing) */
  bool startCapture();                                                                                                                             /* stop capturing */
  bool stopCapture();                                                                                                                              /* start capturing and receiving frames (make sure to call openDevice first) */
  void update();                                                                                                                                   /* some implementations needs to be triggered repeatedly to fetch new frames; so call this as often as possible */

  /* Capture properties */                                  
  int getWidth();                                                                                                                                  /* after opening the device get the heigth */
  int getHeight();                                                                                                                                 /* after opening the device get the width */
  AVPixelFormat getOutPixelFormat();                                                                                                               /* get the pixel format for the output */
  AVPixelFormat getInPixelFormat();                                                                                                                /* get the pixel format for the pixels we get from the grabber */             

  /* Query capabilities */
  bool isPixelFormatSupported(int device, enum AVPixelFormat fmt);                                                                                 /* test if the device supports the given pixel format; this can be handy if you e.g. want to grab in YUV */
  bool isSizeSupported(int device, int width, int height);                                                                                         /* test if the device supports grabbing with the given width/height */
  bool isFrameRateSupported(int device, double fps);                                                                                               /* test if the device supports the given frame rate; use 2 decimals; 30.00, 5.00, 60.00 etc.. */
  std::vector<AVCapability> getCapabilities(int device);                                                                                           /* get a vector with all found capabilities */
  std::vector<AVRational> getSupportedFrameRates(int device,int width, int height, enum AVPixelFormat fmt);                                        /* get a vector with the supported frame rates for the given width, height and pixel format */
  std::vector<enum AVPixelFormat> getSupportedPixelFormats(int device, int width, int height);                                                     /* get a vector with supported pixel formats for the given width and height */
  std::vector<AVSize> getSupportedSizes(int device);                                                                                               /* get a vector with supported width/height */

  /* Print capabilities */       
  void printSupportedPixelFormats(int device);                                                                                                     /* prints the supported pixel formats w/o limiting the list to a pixel format or size */
  void printSupportedPixelFormats(int device, int width, int height);                                                                              /* prints what pixel formats are supported for the given width and height regarding the pixel format */
  void printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt);                                                        /* prints the supported frame rates for the given width/height and pixel format */
  void printSupportedSizes(int device);                                                                                                            /* just prints the supported video sizes */
  void printCapabilities(int device);                                                                                                              /* prints the capabilites for the given device like supported pixel formats, frame rates and sizes */

 private:
  enum State {
    STATE_NONE,
    STATE_OPENED,
    STATE_CAPTURING
  };

  /* Pixel format conversion */
  bool needsSWS();
  AVFrame* allocVideoFrame(enum AVPixelFormat fmt, int w, int h);                                                                                  /* internally used to allocate a AVFrame which in turn is used to convert pixel formats (if necessary) */

 public:
  /* Callback */
  videocapture_frame_callback cb_frame;                                                                                                            /* the user of this class needs to implement it's own callback that will receive the AVFrame* for the input and output. The output (AVFrame* pixelsOut) will only be valid when we have converted the pixel format from VideoCaptureSetings.in_pixel_format to VideoCaptureSettings.out_pixel_format */
  void* cb_user;                                                                                                                                   /* pointer to the user object that get's passed to the callback function you've set in `openDevice` */
  bool has_new_frame;                                                                                                                              /* will be set to true in the callback handler. calling `update()` will trigger the callback, and we also set has_new_frame to false again */

  /* converting between pixel formats */
  VideoCaptureSettings settings;                                                                                                                   /* the VideoCaptureSettings you passed into openDevice */
  SwsContext* sws;                                                                                                                                 /* we use libswscale to convert pixel formats if necessary (which might use hardware acceleration) */
  AVFrame* video_frame_in;                                                                                                                         /* a buffer that will hold the raw, receive data from the video device */
  AVFrame* video_frame_out;                                                                                                                        /* a buffer that will hold the converted video data but only when we the VideoCaptureSettings.in_pixel_format and VideoCaptureSettings.out_pixel_format are different; when they are the same it doesn't make sense to convert them */
  size_t nbytes_in;                                                                                                                                /* number of bytes in the video_frame_in */
  size_t nbytes_out;                                                                                                                               /* number of bytes in the video_frame_out */

  VideoCaptureBase* cap;
  VideoCapture::State state;
};

// VideoCapture extras
// --------------------------------------------------------------------------
inline int VideoCapture::getWidth() {
  return settings.width;
}

inline int VideoCapture::getHeight() {
  return settings.height;
}


// Thin wrapper around implementation
// --------------------------------------------------------------------------

inline int VideoCapture::listDevices() {
  return cap->listDevices();
}

inline bool VideoCapture::startCapture() {

  if(state != VideoCapture::STATE_OPENED) {
    RX_ERROR("Cannot start capturing because we're not opened");
    return false;
  }

  if(state == VideoCapture::STATE_CAPTURING) {
    RX_ERROR("Already capturing...");
    return false;
  }

  if(cap->startCapture()) {
    state = VideoCapture::STATE_CAPTURING;
    cap->setState(VIDCAP_STATE_CAPTURING);
    return true;
  }

  return false;
}

inline bool VideoCapture::stopCapture() {

  if(state != VideoCapture::STATE_CAPTURING) {
    RX_ERROR("Cannot stopCapture() because we're not capturing");
    return false;
  }

  if(cap->stopCapture()) {
    state = VideoCapture::STATE_OPENED;
    cap->setState(VIDCAP_STATE_OPENED);
    return true;
  }

  return false;
}

inline void VideoCapture::update() {
  cap->update();
}

inline bool VideoCapture::isPixelFormatSupported(int device, enum AVPixelFormat fmt) {
  return cap->isPixelFormatSupported(device, fmt);
}

inline bool VideoCapture::isSizeSupported(int device, int width, int height) {
  return cap->isSizeSupported(device, width, height);
};

inline bool VideoCapture::isFrameRateSupported(int device, double fps) {
  return cap->isFrameRateSupported(device, fps);
}

inline std::vector<AVCapability> VideoCapture::getCapabilities(int device) {
  return cap->getCapabilities(device);
}

inline std::vector<AVRational> VideoCapture::getSupportedFrameRates(int device,int width, int height, enum AVPixelFormat fmt) {
  return cap->getSupportedFrameRates(device, width, height, fmt);
}

inline std::vector<enum AVPixelFormat> VideoCapture::getSupportedPixelFormats(int device, int width, int height) {
  return cap->getSupportedPixelFormats(device, width, height);
}

inline std::vector<AVSize> VideoCapture::getSupportedSizes(int device) {
  return cap->getSupportedSizes(device);
}

inline void VideoCapture::printSupportedPixelFormats(int device) {
 cap->printSupportedPixelFormats(device);
}

inline void VideoCapture::printSupportedPixelFormats(int device, int width, int height) {
  cap->printSupportedPixelFormats(device, width, height);
}

inline void VideoCapture::printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt) {
  cap->printSupportedFrameRates(device, width, height, fmt);
}

inline void VideoCapture::printSupportedSizes(int device) {
  cap->printSupportedSizes(device);
}

inline void VideoCapture::printCapabilities(int device) {
  cap->printCapabilities(device);
}

inline AVPixelFormat VideoCapture::getInPixelFormat() {
  return settings.in_pixel_format;
}

inline AVPixelFormat VideoCapture::getOutPixelFormat() {
  return settings.out_pixel_format;
}


#endif

