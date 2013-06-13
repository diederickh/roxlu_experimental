#ifndef ROXLU_V4L2_CAPTURE_H
#define ROXLU_V4L2_CAPTURE_H

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <libudev.h>
#include <locale.h>
#include <unistd.h>
}

#include <string>
#include <vector>
#include <videocapture/rx_capture.h>
#include <videocapture/Types.h>

#define V4L2_MAX_QUERYABLE_INPUTS 16

enum LinuxCaptureIOMethods {
  LINCAP_IO_METHOD_READ = 1 << 0,
  LINCAP_IO_METHOD_MMAP = 1 << 1,
  LINCAP_IO_METHOD_USERPTR = 1 << 2
};

// Information about a capture device, see http://www.ideasonboard.org/uvc/ for more info
// Good example: http://linuxtv.org/downloads/v4l-dvb-apis/capture-example.html
struct LinuxCaptureDevice { 
  std::string path;
  std::string id_vendor;
  std::string id_product;
};

struct LinuxCaptureBuffer {
  void* start;
  size_t length;
};

class VideoCaptureV4L2 {
 public:
  VideoCaptureV4L2();
  ~VideoCaptureV4L2();
  void setup();
  int listDevices();
  int printVerboseInfo();
  int openDevice(int device, int w, int h, VideoCaptureFormat capFormat);
  int isFormatSupported(VideoCaptureFormat fmt, int w, int h, int applyFormat = 0);
  int startCapture();
  int stopCapture();
  void update();
  int closeDevice();
  int getWidth();
  int getHeight();
  void setFrameCallback(rx_capture_frame_cb cb, void* user);
 private:
  bool initMMAP();
  int printDeviceInfo(int dev);
  void printV4L2Format(v4l2_format* fmt);
  std::string v4l2BufTypeToString(int type);
  std::string v4l2PixelFormatToString(int pixfmt);
  std::vector<LinuxCaptureDevice> getDeviceList();
  int videoCaptureFormatToPixelFormat(VideoCaptureFormat fmt);
 public:
  rx_capture_frame_cb frame_cb;
  void* frame_user;
 private:
  int width; // width of webcam images
  int height; // height of webcam images
  int fd;
  bool is_opened;
  int io_method;
  std::vector<LinuxCaptureBuffer*> buffers;
};

inline int VideoCaptureV4L2::getWidth() {
  return width;
}

inline int VideoCaptureV4L2::getHeight() {
  return height;
}

inline void VideoCaptureV4L2::setFrameCallback(rx_capture_frame_cb cb, void* user) {
  frame_cb = cb;
  frame_user = user;
}
#endif
