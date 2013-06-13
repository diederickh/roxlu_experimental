#ifndef ROXLU_VIDEOCAPTURE_V4L2_H
#define ROXLU_VIDEOCAPTURE_V4L2_H

extern "C" {
#  include <assert.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <fcntl.h>
#  include <errno.h>
#  include <sys/mman.h>
#  include <sys/ioctl.h>
#  include <asm/types.h>
#  include <linux/videodev2.h>
#  include <libudev.h>
#  include <locale.h>
#  include <unistd.h>
}

#include <videocapture/Types.h>
#include <string>
#include <vector>
#include <videocapture/VideoCaptureBase.h>
#include <videocapture/linux/v4l2/VideoCaptureV4L2Utils.h>
#include <videocapture/linux/v4l2/VideoCaptureV4L2Types.h>

static int v4l2_ioctl(int fh, int request, void* arg);

class VideoCaptureV4L2 : public VideoCaptureBase {
 public:
  VideoCaptureV4L2();
  ~VideoCaptureV4L2();
  
  // Camera control
  int listDevices();
  bool openDevice(int dx, VideoCaptureSettings cfg);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  // IO Methods
  bool readFrame();                                                                    /* when there is a frame, this will read it */
  bool initializeMMAP(int fd);                                                         /* initialize mmap IO for the given device descriptor */
  bool shutdownMMAP();

  // Capabilities
  std::vector<AVCapability> getCapabilities(int dx);
  bool getCapability(int fd, struct v4l2_capability* caps);                         
  bool printCurrentCaptureFormat(int fd);
  
  // Device related
  int openDevice(std::string path);                                                    /* open the device and return a descriptor; path is the device devpath */
  bool closeDevice(int fd);                                                            /* close the given device descriptor; is use when opening/closing multiple devices to test e.g. capabilities; get info on the devices, ec.. */
  bool getDevice(int dx, VideoCaptureV4L2Device& result);                              /* get some gasic info for the given device index */
  bool getDeviceInfo(const char* path, VideoCaptureV4L2Device& result);                /* get device info (see VideoCaptureV4L2Types.h) */
  bool getDeviceInfo(int dx, VideoCaptureV4L2Device& result);                          /* get name, vendor, version */
  bool setCaptureFormat(int fd, int width, int height, AVPixelFormat pixelFormat);     /* make sure to use the given width, height and pixelformat */

 private:
  VideoCaptureV4L2Device capture_device;
  VideoCaptureV4L2IOMethod io_method;                                                   /* the i/o method we use; only support MMAP for now */
  std::vector<VideoCaptureV4L2Buffer*> capture_buffers;                                 /* the mmapped memory that we use with the MMAP io method */
  int capture_device_fd;
};

#endif
