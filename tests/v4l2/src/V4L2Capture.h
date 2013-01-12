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

class V4L2Capture {
 public:
  V4L2Capture();
  ~V4L2Capture();
  void setup();
  int listDevices();
  int printVerboseInfo();
  int openDevice(int device);
  int startCapture();
  void update();
  int closeDevice();
 private:
  bool initMMAP();
  int printDeviceInfo(int dev);
  std::vector<LinuxCaptureDevice> getDeviceList();
 private:
  int fd;
  bool is_opened;
  int io_method;
  std::vector<LinuxCaptureBuffer*> buffers;
};

#endif
