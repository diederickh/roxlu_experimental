/*

  This webcam class combines the `VideoCapture` and `GPUImage/Drawer`
  features in one simple class which makes it easier to draw the captured 
  video using an optimized path; you can use YUYV422/YUV422/etc.. for 
  optimized throughput. 


*/
#ifndef ROXLU_VIDEOCAPTURE_WEBCAM_H
#define ROXLU_VIDEOCAPTURE_WEBCAM_H

#include <videocapture/VideoCapture.h>
#include <gpu/GPUImage.h>
#include <gpu/GPUDrawer.h>

// ---------------------------------------------------------

// CALLED FROM OTHER THREAD (!)
void webcam_frame_callback(AVFrame* in, size_t nbytesin, AVFrame* out, size_t nbytesout, void* user);

// ---------------------------------------------------------
class WebcamListener {
 public:
  virtual void onWebcamPixelsUpdated(char* inpixels, size_t nbytesin, char* outpixels, size_t nbytesout) = 0;            /* will get called when we got new pixels data from the capturer; the `pixel` argument will contain the pixels in the output pixel format */
};
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

  /* Listeners */
  void addListener(WebcamListener* listener);

  /* Capture control */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg, videocapture_frame_callback frameCB = NULL, void* user = NULL);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  /* draw! & position */
  void setFlipVertical(bool flip);
  void setFlipHorizontal(bool flip);
  void setRotation(float radians);
  void setPosition(float x, float y, float z = 0.0);
  void setSize(float w, float h);
  void draw();

  /* Capture properties */
  int getWidth();
  int getHeight();
  AVPixelFormat getOutPixelFormat();
  AVPixelFormat getInPixelFormat();

  /* Capabilities */
  bool isPixelFormatSupported(int device, enum AVPixelFormat fmt);
  bool isSizeSupported(int device, int w, int h);
  bool isFrameRateSupported(int device, double fps);
  void printCapabilities(int device);
  void printSupportedPixelFormats(int device);
  void printSupportedPixelFormats(int device, int w, int h);
  void printSupportedFrameRates(int device, int width, int height, enum AVPixelFormat fmt);

 public:
  std::vector<WebcamListener*> listeners;                           
  volatile bool has_new_pixels;
  int num_out_bytes;
  int num_in_bytes;
  char* out_pixels;
  char* in_pixels;
  VideoCapture cap;
  videocapture_frame_callback cb_frame;
  void* cb_user;
  GLuint* gpu_tex;
  GPUImage gpu_image;
  GPUDrawer gpu_drawer;
  VideoCaptureSettings settings;
};

inline void Webcam::addListener(WebcamListener* listener) {
  listeners.push_back(listener);
}

inline int Webcam::listDevices() {
  return cap.listDevices();
}

inline bool Webcam::closeDevice() {

  // reset our own main-treaded-buffer
  if(out_pixels) {
    delete[] out_pixels;
    out_pixels = NULL;
    num_out_bytes = 0;
    has_new_pixels = false;
  }

  if(gpu_tex) {
    gpu_image.deleteTextures(gpu_tex);
    gpu_tex = NULL;
  }

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

inline AVPixelFormat Webcam::getInPixelFormat() {
  return cap.getInPixelFormat();
}

inline AVPixelFormat Webcam::getOutPixelFormat() {
  return cap.getOutPixelFormat();
}

inline void Webcam::setPosition(float x, float y, float z) {
  gpu_drawer.setPosition(x, y, z);
}

inline void Webcam::setSize(float w, float h) {
  gpu_drawer.setSize(w, h);
}

inline void Webcam::setFlipVertical(bool flip) {
  gpu_drawer.setFlipVertical(flip);
}

inline void Webcam::setFlipHorizontal(bool flip) {
  gpu_drawer.setFlipHorizontal(flip);
}

inline void Webcam::setRotation(float radians) {
  gpu_drawer.setRotation(radians);
}

#endif
