#ifndef ROXLU_VIDEOCAPTURE_INTERFACE_H
#define ROXLU_VIDEOCAPTURE_INTERFACE_H

#include <vector>
#include <set>
#include <videocapture/Types.h>

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
}

#define VIDCAP_STATE_NONE 0
#define VIDCAP_STATE_OPENED 1
#define VIDCAP_STATE_CAPTURING 2

class VideoCaptureBase {
 public:
  VideoCaptureBase();                                                                                    /* c'tor - implementation should initailize the necessary sdks here */
  virtual ~VideoCaptureBase();                                                                           /* virtual d'tor - make sure that you free all mem here */

  /* CAMERA CONTROL */
  virtual int listDevices() = 0;                                                                         /* logs the devices + returns the number of found devices */
  virtual bool openDevice(int device, VideoCaptureSettings cfg) = 0;                                     /* open the given device using the `VideoCaptureSettings` */
  virtual bool closeDevice() = 0;                                                                        /* closes the currently open device. will return false error or if the device isn't opened yet */
  virtual bool startCapture() = 0;                                                                       /* start receiving the video frames */
  virtual bool stopCapture() = 0;                                                                        /* stop captureing from the device */
  virtual void update() = 0;                                                                             /* call this as often as you can; it will process the capture buffers (if available in implementation) */
  virtual void setFrameCallback(videocapture_frame_cb frameCB, void* user);                              /* set the function that needs to be called on every frame */

  /* STATE */
  void setState(int state);                                                                              /* set the state to one of the VIDCAP_STATE_*, this is done by the wrapper class: VideoCapture */
  int getState();                                                                                        /* get the current state */
                                                                                       
  /* UTILS */                                                                          
  bool isPixelFormatSupported(int device, enum AVPixelFormat fmt);                                       /* check if the given pixel format is supported */
  bool isSizeSupported(int device, int width, int height);                                               /* check if the given size is supported by the device */
  bool isFrameRateSupported(int device, double fps);                                                     /* check if the given frame rate is supported; fps must be 2 decimals significant, 29.97, 7.50, 30.00 */

  /* CAPABILITIES */
  virtual std::vector<AVCapability> getCapabilities(int device) = 0;                                     /* query all capabilities of the device */
  bool getBestMatchingCapability(int device, VideoCaptureSettings cfg,  AVCapability& result);           /* this function will return the capability which best matches the given capture settings. It will set the given AVCapabilty */
  std::vector<AVPixelFormat> getSupportedPixelFormats(int device, int width, int height);
  std::vector<AVRational> getSupportedFrameRates(int device, int width, int height, AVPixelFormat fmt);
  std::vector<AVSize> getSupportedSizes(int device);
  
  /* DEBUG - VERBOSE */
  void printSupportedPixelFormats(int device);                                                           /* logs supported pixel formats; independent from the width, height or fps */
  void printSupportedPixelFormats(int device, int width, int height);                                    /* logs the supported pixel formats */
  void printSupportedFrameRates(int device, int width, int height,  enum AVPixelFormat fmt);             /* prints the supported frame rates for the given device, width, height and pixel format */
  void printSupportedSizes(int device);                                                                  /* logs the supported sizes to the console */
  void printCapabilities(int device);                                                                    /* print capabilities */
 
 public:
  VideoCaptureSettings settings;
  videocapture_frame_cb cb_frame;
  void* cb_user;
  int state;
};

// --------------------------------------------------------------------------

inline VideoCaptureBase::VideoCaptureBase() 
                        :cb_frame(NULL)
                        ,cb_user(NULL)
                        ,state(VIDCAP_STATE_NONE)
{
}

inline VideoCaptureBase::~VideoCaptureBase() {
}

inline void VideoCaptureBase::setFrameCallback(videocapture_frame_cb frameCB, void* user) {
  cb_frame = frameCB;
  cb_user = user;
}

inline void VideoCaptureBase::setState(int st) {
  state = st;
}

inline int VideoCaptureBase::getState() {
  return state;
}

#endif
