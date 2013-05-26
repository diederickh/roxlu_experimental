#ifndef ROXLU_VIDEOCAPTURE_H
#define ROXLU_VIDEOCAPTURE_H

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <string.h> 
#include <roxlu/core/Log.h>
#include <videocapture/Types.h>

#if defined(__APPLE__)
#  include <videocapture/mac/VideoCaptureMac.h>
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


typedef void (*videocapture_frame_callback)(AVFrame* pixelsIn, size_t nbytesIn,                        /* this function will be calls when we've processed a frame pixelsIn are the raw bytes; pixelsOut are the pixels converted to pixels_out_format (if set) */
                                            AVFrame* pixelsOut, size_t nbytesOut, 
                                            void* user);

void videocapture_process_frame_callback(void* pixels, size_t nbytes, void* user);                  /* will be called by the implementation */

class VideoCapture {
 public:
  VideoCapture();
  ~VideoCapture();

  /* Capture control */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg, 
                  videocapture_frame_callback frameCB, void* user);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  /* Capture properties */
  int getWidth();                                                    
  int getHeight();

  /* Query capabilities */
  bool isPixelFormatSupported(int device, enum AVPixelFormat fmt);
  bool isSizeSupported(int device, int width, int height);
  bool isFrameRateSupported(int device, double fps);
  std::vector<AVCapability> getCapabilities(int device);  
  std::vector<AVRational> getSupportedFrameRates(int device,int width, int height, enum AVPixelFormat fmt);
  std::vector<enum AVPixelFormat> getSupportedPixelFormats(int device, int width, int height);
  std::vector<AVSize> getSupportedSizes(int device);

  /* Print capabilities */
  void printSupportedPixelFormats(int device);  
  void printSupportedPixelFormats(int device, int width, int height); 
  void printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt);
  void printSupportedSizes(int device);      
  void printCapabilities(int device);       

 private:
  /* Pixel format conversion */
  bool needsSWS();
  AVFrame* allocVideoFrame(enum AVPixelFormat fmt, int w, int h);

 public:
  /* callback */
  videocapture_frame_callback cb_frame;
  void* cb_user;                                        /* pointer to the user object that get's passed to the callback function you've set in `openDevice` */
  bool has_new_frame;                                   /* will be set to true in the callback handler. calling `update()` will trigger the callback, and we also set has_new_frame to false again */

  /* converting between pixel formats */
  VideoCaptureSettings settings;                        
  SwsContext* sws;
  AVFrame* video_frame_in;    
  AVFrame* video_frame_out;   
  size_t nbytes_in;
  size_t nbytes_out;

#if defined(__APPLE__)
  VideoCaptureMac cap;
#endif


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
  return cap.listDevices();
}

inline bool VideoCapture::closeDevice() {
  return cap.closeDevice();
}

inline bool VideoCapture::startCapture() {
  return cap.startCapture();
}

inline bool VideoCapture::stopCapture() {
  return cap.stopCapture();
}

inline void VideoCapture::update() {
  cap.update();
}

inline bool VideoCapture::isPixelFormatSupported(int device, enum AVPixelFormat fmt) {
  return cap.isPixelFormatSupported(device, fmt);
}

inline bool VideoCapture::isSizeSupported(int device, int width, int height) {
  return cap.isSizeSupported(device, width, height);
};

inline bool VideoCapture::isFrameRateSupported(int device, double fps) {
  return cap.isFrameRateSupported(device, fps);
}

inline std::vector<AVCapability> VideoCapture::getCapabilities(int device) {
  return cap.getCapabilities(device);
}

inline std::vector<AVRational> VideoCapture::getSupportedFrameRates(int device,int width, int height, enum AVPixelFormat fmt) {
  return cap.getSupportedFrameRates(device, width, height, fmt);
}

inline std::vector<enum AVPixelFormat> VideoCapture::getSupportedPixelFormats(int device, int width, int height) {
  return cap.getSupportedPixelFormats(device, width, height);
}

inline std::vector<AVSize> VideoCapture::getSupportedSizes(int device) {
  return cap.getSupportedSizes(device);
}

inline void VideoCapture::printSupportedPixelFormats(int device) {
 cap.printSupportedPixelFormats(device);
}

inline void VideoCapture::printSupportedPixelFormats(int device, int width, int height) {
  cap.printSupportedPixelFormats(device, width, height);
}

inline void VideoCapture::printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt) {
  cap.printSupportedFrameRates(device, width, height, fmt);
}

inline void VideoCapture::printSupportedSizes(int device) {
  cap.printSupportedSizes(device);
}

inline void VideoCapture::printCapabilities(int device) {
  cap.printCapabilities(device);
}

#endif

