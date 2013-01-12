#ifndef ROXLU_VIDEOCAPTURE_BASE_H
#define ROXLU_VIDEOCAPTURE_BASE_H

#include <algorithm>
#include <videocapture/rx_capture.h>
#include <stdio.h>
#include <string.h> // memcpy

extern void video_capture_callback(void* pixels, size_t nbytes, void* user);

class VideoCapture {
 public:
  VideoCapture();
  ~VideoCapture();
  int listDevices();
  int printVerboseInfo();
  int openDevice(int device, int w, int h, VideoCaptureFormat fmt);
  int startCapture();
  int isFormatSupported(VideoCaptureFormat fmt, int w, int h, int set = 0); 
  unsigned char* getPtr(); // get pointer to 'bytes'
  size_t getNumBytes(); // get number of bytes in 'bytes'
  bool hasNewData(); // returns true when we received data from capture device
  void copyData(char* dest); // copies data + sets 'has_new_data' to false. Make sure 'dest' can hold getNumBytes
  int getWidth();
  int getHeight();
  void resetHasNewData(); // tells us that we've read the last frame
 private: 
  void setupShader();
  void setupBuffer();
 private:
  rx_capture_t* c;
 public:
  size_t nbytes; // number of bytes copied to 'bytes'
  bool has_new_data;
  size_t allocated_bytes;
  unsigned char* bytes;
  rx_capture_t capture;
};

inline unsigned char* VideoCapture::getPtr() {
  return bytes;
}

inline size_t VideoCapture::getNumBytes() {
  return nbytes;
}

inline bool VideoCapture::hasNewData() {
  return has_new_data;
}

inline void VideoCapture::copyData(char *dest) {
  memcpy(dest, bytes, nbytes);
}

inline void VideoCapture::resetHasNewData() {
  has_new_data = false;
}

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

inline int VideoCapture::openDevice(int device, int w, int h, VideoCaptureFormat fmt) {
  capture.set_frame_callback(&capture, video_capture_callback, this);
  return capture.open_device(&capture, device, w, h, fmt);
}

inline int VideoCapture::startCapture() {
  return capture.capture_start(&capture);
}

#endif

