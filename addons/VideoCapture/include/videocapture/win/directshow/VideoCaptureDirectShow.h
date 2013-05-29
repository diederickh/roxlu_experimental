#ifndef ROXLU_VIDEOCAPTURE_DIRECTSHOW_2_H
#define ROXLU_VIDEOCAPTURE_DIRECTSHOW_2_H

extern "C" {
#  include <libavformat/avformat.h>
#  include <libavutil/avutil.h>
}


#include <stdio.h>
#include <windows.h>
#include <dshow.h>
#include <algorithm>
#include <vector>
#include <string>
#include <videocapture/win/directshow/qedit.h>
#include <videocapture/win/directshow/VideoCaptureDirectShowCB.h>
#include <videocapture/Types.h>
#include <videocapture/VideoCaptureBase.h>

// DIRECTSHOW CAPTURE
// --------------------------------------------------------------------------------------
class VideoCaptureDirectShow2 : public VideoCaptureBase {
 public:
  VideoCaptureDirectShow2();
  ~VideoCaptureDirectShow2();

  /* Capture control */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  /* Capabilities */
  std::vector<AVCapability> getCapabilities(int device);
  
 private:

  /* Helpers */
  bool createDeviceFilter(int device, IBaseFilter** deviceFilter);
  bool createDeviceMoniker(IEnumMoniker** moniker);
  bool createGraphBuilder(IGraphBuilder** builder);
  bool createCaptureGraphBuilder(ICaptureGraphBuilder2** captureBuilder);
  bool createStreamConfig(ICaptureGraphBuilder2* captureBuilder, IBaseFilter* startFilter, IAMStreamConfig** config);
  bool createSampleGrabberFilter(IBaseFilter** grabberFilter);
  bool createSampleGrabber(IBaseFilter* grabberFilter, ISampleGrabber** sampleGrabber);
  bool createMediaControl(IGraphBuilder* graphBuilder, IMediaControl** mediaControl);
  bool createMediaEvent(IGraphBuilder* graphBuilder, IMediaEventEx** mediaEvent);
  bool createNullRendererFilter(IBaseFilter** nullRendererFilter);
  bool initCaptureGraphBuilderForDevice(int device, IBaseFilter** deviceFilter, IGraphBuilder** graphBuilder, ICaptureGraphBuilder2** captureBuilder); 
  bool initCaptureGraphBuilder(IGraphBuilder** graphBuilder, ICaptureGraphBuilder2** captureBuilder);
  bool addSampleGrabberFilter(IGraphBuilder* graphBuilder, IBaseFilter* grabberFilter);
  bool addNullRendererFilter(IGraphBuilder* graphBuilder, IBaseFilter* nullRendererFilter);
  bool setSampleGrabberMediaType(ISampleGrabber* sampleGrabber, AVPixelFormat pixelFormat);
  bool setDeviceFilterMediaType(ICaptureGraphBuilder2* captureBuilder, IBaseFilter* deviceFilter, AVCapability cap);             /* Tries to setup the deviceFilter (which represents a capture device), for the given width, height and pixelFormat. To do this we need a IAMStreamConfig which will we allocate+free */
  bool setSampleGrabberCallback(ISampleGrabber* sampleGrabber, ISampleGrabberCB* sampleGrabberCB);                               /* Attaches the callback instance to the grabber filter; */               

  /* Utils */
  void printConnectedMediaTypeForSampleGrabber(ISampleGrabber* sampleGrabber);
  void printMediaType(AM_MEDIA_TYPE* mt);
  void deleteMediaType(AM_MEDIA_TYPE* mt);
  GUID libavPixelFormatToMediaSubTypeGUID(AVPixelFormat fmt);
  AVPixelFormat mediaSubTypeGUIDToLibavPixelFormat(GUID guid);
  std::string mediaFormatMajorTypeToString(GUID type);
  std::string mediaFormatFormatTypeToString(GUID type);
  std::string mediaFormatSubTypeToString(GUID type);

 private:
  IGraphBuilder* graph_builder;
  ICaptureGraphBuilder2* capture_graph_builder;
  IBaseFilter* device_filter;
  IBaseFilter* sample_grabber_filter;
  IBaseFilter* null_renderer_filter;
  ISampleGrabber* sample_grabber;
  IMediaControl* media_control;
  IMediaEventEx* media_event;
  VideoCaptureDirectShowCB* capture_cb;
};

// UTILS
// --------------------------------------------------------------------------------------
template <class T> void safeReleaseDirectShow(T **v);
template <class T> std::string directShowToHex(const T& in, int width = 2 * sizeof(T));
std::string GUIDtoString(const GUID& id);

#endif
