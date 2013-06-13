#ifndef ROXLU_VIDEOCAPTURE_MAC_H
#define ROXLU_VIDEOCAPTURE_MAC_H

#include <videocapture/Types.h>
#include <videocapture/VideoCaptureBase.h>
#include <videocapture/mac/VideoCaptureMacInterface.h>

#define ERR_CAP_MAC_CANNOT_ALLOC "Cannot allocate the AVFoundation grabber"
#define ERR_CAP_MAC_PIXFMT_NOT_ALLOC "Cannot test if pixel format is supported because the avfoundation capturer hasn't been allocated"
#define ERR_CAP_MAC_CAP_NOT_ALLOC "The AVFoundation grabber is not allocated"

class VideoCaptureMac : public VideoCaptureBase {
 public:
  VideoCaptureMac();
  ~VideoCaptureMac();

  /* CAMERA CONTROL */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();
  void setFrameCallback(videocapture_frame_cb frameCB, void* user);

  /* CAPABILITIES */
  std::vector<AVCapability> getCapabilities(int device);

 private:
  void* cap;
};

#endif
