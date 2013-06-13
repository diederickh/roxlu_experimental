/*

  This webcam class combines the `VideoCapture` and `VideoCaptureGLSurface`
  features in one simple class which makes it easier to draw the captured 
  video to your window.


*/
#ifndef ROXLU_VIDEOCAPTURE_WEBCAM_H
#define ROXLU_VIDEOCAPTURE_WEBCAM_H

#include <videocapture/VideoCaptureGLSurface.h>
#include <videocapture/VideoCapture.h>

// ---------------------------------------------------------

void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user);

// ---------------------------------------------------------

class Webcam {
 public:

#if defined(_WIN32)
  Webcam(VideoCaptureImplementation imp = VIDEOCAPTURE_DIRECTSHOW);
#elif defined(__APPLE__)
  Webcam(VideoCaptureImplementation imp = VIDEOCAPTURE_AVFOUNDATION);
#elif defined(__linux)  
  Webcam(VideoCaptureImplementation imp = VIDEOCAPTURE_V4L2);
#else
  Webcam(VideoCaptureImplementation imp);
#endif
  ~Webcam();

  /* Capture control */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB = NULL, void* user = NULL);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  /* draw! */
  void draw(int x, int y, int w, int h);

  /* Capture properties */
  int getWidth();
  int getHeight();

  /* Capabilities */
  bool isPixelFormatSupported(int device, enum AVPixelFormat fmt);
  bool isSizeSupported(int device, int w, int h);
  bool isFrameRateSupported(int device, double fps);
  void printCapabilities(int device);
  void printSupportedPixelFormats(int device);
  void printSupportedPixelFormats(int device, int w, int h);
  void printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt);

 public:
  VideoCapture cap;
  VideoCaptureGLSurface surface;
  videocapture_frame_callback cb_frame;
  void* cb_user;
};

inline Webcam::Webcam(VideoCaptureImplementation imp)
              :cap(imp)
             ,cb_user(NULL)
             ,cb_frame(NULL)
{
}

inline int Webcam::listDevices() {
  return cap.listDevices();
}

inline bool Webcam::closeDevice() {
  return cap.closeDevice();
}

inline bool Webcam::startCapture() {
  return cap.startCapture();
}

inline bool Webcam::stopCapture() {
  return cap.stopCapture();
}

inline int Webcam::getWidth() {
  return cap.getWidth();
}

inline int Webcam::getHeight() {
  return cap.getHeight();
}

inline bool Webcam::isPixelFormatSupported(int device, enum AVPixelFormat fmt) {
  return cap.isPixelFormatSupported(device, fmt);
}

inline bool Webcam::isSizeSupported(int device, int w, int h) {
  return cap.isSizeSupported(device, w, h);
}

inline bool Webcam::isFrameRateSupported(int device, double fps) {
  return cap.isFrameRateSupported(device, fps);
}

inline void Webcam::printCapabilities(int device) {
  cap.printCapabilities(device);
}

inline void Webcam::printSupportedPixelFormats(int device) {
  cap.printSupportedPixelFormats(device);
}

inline void Webcam::printSupportedPixelFormats(int device, int w, int h) {
  cap.printSupportedPixelFormats(device, w, h);
}

inline void Webcam::printSupportedFrameRates(int device, int w, int h, enum AVPixelFormat fmt) {
  cap.printSupportedFrameRates(device, w, h, fmt);
}


#endif
