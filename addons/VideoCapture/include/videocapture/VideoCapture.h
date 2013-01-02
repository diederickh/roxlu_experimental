#ifndef ROXLU_VIDEOCAPTURE_BASE_H
#define ROXLU_VIDEOCAPTURE_BASE_H

//#include <roxlu/Roxlu.h>
#include <algorithm>
#include <videocapture/rx_capture.h>
#include <stdio.h>

extern void video_capture_callback(void* pixels, size_t nbytes, void* user);

class VideoCapture {
 public:
  VideoCapture();
  ~VideoCapture();
  int listDevices();
  int printVerboseInfo();
  int openDevice(int device);
  int startCapture();
  unsigned char* getPtr(); // get pointer to 'bytes'
  size_t getNumBytes(); // get number of bytes in 'bytes'
  bool hasNewData(); // returns true when we received data from capture device
  void copyData(char* dest); // copies data + sets 'has_new_data' to false. Make sure 'dest' can hold getNumBytes
  int getWidth();
  int getHeight();
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
  has_new_data = false;
}

inline int VideoCapture::getWidth() {
  return rx_capture_get_width(c);
}

inline int VideoCapture::getHeight() {
  return rx_capture_get_height(c);
}
#endif

