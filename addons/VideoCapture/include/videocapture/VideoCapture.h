#ifndef ROXLU_VIDEOCAPTURE_BASE_H
#define ROXLU_VIDEOCAPTURE_BASE_H

#include <algorithm>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <string.h> // memcpy
#include <roxlu/core/Log.h>

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
  //#  include <libavutil/imgutils.h>
  //#  include <libavutil/mathematics.h>
#  include <libavcodec/avcodec.h>
#  include <libswscale/swscale.h>

}


extern void video_capture_callback(void* pixels, size_t nbytes, void* user);

class VideoCapture {
 public:
  VideoCapture();
  ~VideoCapture();
  int listDevices();
  int printVerboseInfo();
  //  int openDevice(int device, int w, int h, VideoCaptureFormat fmt);
  int openDevice(int device, VideoCaptureSettings cfg);
  int closeDevice();
  int startCapture();
  void update();
  int isFormatSupported(VideoCaptureFormat fmt, int w, int h, int set = 0); 
  // CHANGED: see getNewDataPtr()
  //unsigned char* getPtr(); // get pointer to 'bytes'
  size_t getNumBytes(); // get number of bytes in 'bytes'
  bool hasNewData(); // returns true when we received data from capture device
  void copyData(char* dest); // copies data + sets 'has_new_data' to false. Make sure 'dest' can hold getNumBytes
  int getWidth();
  int getHeight();
  // use getNewDataPtr() --> resets automatically!
  //  void resetHasNewData(); // tells us that we've read the last frame
  unsigned char* getNewDataPtr();
  VideoCaptureFormat getFormat();
 private: 
  bool needsSWS();
  AVFrame* allocVideoFrame(enum AVPixelFormat fmt, int w, int h);
  void setupShader();
  void setupBuffer();
 private:
  rx_capture_t* c;
 public:
  VideoCaptureSettings settings;
  size_t nbytes; // number of bytes copied to 'bytes'
  bool has_new_data;
  size_t allocated_bytes;
  unsigned char* bytes; /* output bytes as defined by your VideoCaptureSettings.out_pixel_format */
  unsigned char* pixels_in; /* raw input bytes as we received from the capturer */
  unsigned char* pixels_out; /* converted pixels as defined in VideoCaptureSettings.in_pixel_format */
  rx_capture_t capture;
  VideoCaptureFormat fmt;

  /* converting between pixel formats */
  SwsContext* sws;
  AVFrame* video_frame_in;     /* tmp - used to convert input image */
  AVFrame* video_frame_out;    /* tmp - used to convert input image */
};

//inline unsigned char* VideoCapture::getPtr() {
//  return bytes;
//}

inline unsigned char* VideoCapture::getNewDataPtr() {
  has_new_data = false;
  return bytes;
}

inline size_t VideoCapture::getNumBytes() {
  return nbytes;
}

inline void VideoCapture::update() {
  capture.update(&capture);
}

inline bool VideoCapture::hasNewData() {
  return has_new_data;
}

inline void VideoCapture::copyData(char *dest) {
  memcpy(dest, bytes, nbytes);
}

//inline void VideoCapture::resetHasNewData() {
//  has_new_data = false;
//}

inline int VideoCapture::getWidth() {
  return capture.get_width(&capture);
}

inline int VideoCapture::getHeight() {
  return capture.get_height(&capture);
}

inline int VideoCapture::isFormatSupported(VideoCaptureFormat fmt, int w, int h, int set) {
  return capture.is_format_supported(&capture, fmt, w, h, set);
}

inline int VideoCapture::listDevices() {
  return capture.list_devices(&capture);
}

inline int VideoCapture::printVerboseInfo() {
  return capture.print_verbose_info(&capture);
}

//inline int VideoCapture::openDevice(int device, int w, int h, VideoCaptureFormat format) {
inline int VideoCapture::openDevice(int device, VideoCaptureSettings cfg) { 
  settings = cfg;

  if(needsSWS()) {
    sws = sws_getContext(settings.width, settings.height, settings.in_pixel_format,
                         settings.width, settings.height, settings.out_pixel_format,
                         SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if(!sws) {
      RX_ERROR("Cannot create an sws context that we need to convert between pixel formats");
      return 0;
    }
    
    video_frame_in = allocVideoFrame(settings.in_pixel_format, settings.width, settings.height);
    video_frame_out = allocVideoFrame(settings.out_pixel_format, settings.width, settings.height);

    int size = 0;
    size = avpicture_get_size(settings.in_pixel_format, settings.width, settings.height);
    pixels_in = new unsigned char[size];
    RX_VERBOSE("SIZE: %d", size);

    size = avpicture_get_size(settings.out_pixel_format, settings.width, settings.height);
    pixels_out = new unsigned char[size];
    RX_VERBOSE("SIZE: %d", size);
    
    //   int nbytes_per_image = avpicture_get_size(settings.in_pixel_format, settings.in_w, settings.in_h);
  }

  capture.set_frame_callback(&capture, video_capture_callback, this);
  fmt = VC_FMT_YUYV422;
  //fmt = VC_FMT_UYVY422;
  //fmt = VC_FMT_RGB24;
  return capture.open_device(&capture, device, settings.width, settings.height, fmt);
}

inline int VideoCapture::closeDevice() {
  return capture.close_device(&capture);
}

inline int VideoCapture::startCapture() {
  return capture.capture_start(&capture);
}

inline bool VideoCapture::needsSWS() {
  return true; // tmp - we need to start using a VideoCaptureSettings object to convert from->to 
}

inline VideoCaptureFormat VideoCapture::getFormat() {
  return fmt;
}



#endif

