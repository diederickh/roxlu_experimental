#ifndef ROXLU_VIDEOCAPTURE_DIRECTSHOW_H
#define ROXLU_VIDEOCAPTURE_DIRECTSHOW_H

#pragma warning(once : 4995)
//#define _ATL_NO_UUIDOF 

#include <stdio.h>
#include <windows.h>
#include <dshow.h>
#include <vector>
#include <string>
#include "qedit.h"

/**
 * Experimental Video Capture on Windows
 *
 * Setup:
 * - Add strmiids.lib as linker input
 *
 * 
 * References: 
 * - http://msdn.microsoft.com/en-us/library/windows/desktop/dd375472(v=vs.85).aspx
 *   - PIN_CATEGORY_PREVIEW: for viewing; frames are dropped ; a bit latency 
 *   - PIN_CATEGORY_CAPTURE: faster; for file writing
 */

#define LOG(x) printf x
#define RETURN_IF_FAILED(hr, msg, rval) { if(FAILED(hr)) { printf(msg); return rval; } }


// ISampleGrabberCB implementation: http://msdn.microsoft.com/en-us/library/windows/desktop/dd376985(v=vs.85).aspx
class VideoCaptureDirectShowCallback : public ISampleGrabberCB {
 public:
  HRESULT SampleCB(double timestamp, IMediaSample* sample); // non buffered i/o
  HRESULT BufferCB(double timestamp, BYTE* buffer, long size); // buffered i/o
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  STDMETHODIMP QueryInterface(REFIID riid, void** refD);
};

struct VideoCaptureDirectShowDevice {
  std::string name;
};

class VideoCaptureDirectShow {
public:
  VideoCaptureDirectShow(void);
  ~VideoCaptureDirectShow(void);
  int listDevices();
  int printVerboseInfo();
  int openDevice(int dev);
  int startCapture();
  int stopCapture();
 private:
  void close();
  bool initCaptureGraphBuilder();
  void fillDeviceInformationList(IEnumMoniker* enumerator);
  void nukeDownStream(IBaseFilter* filter); // disconnect all pins from this filter and children (cleanup)
  void destroyGraph(); // cleans up the complete filter graph
  void printAmMediaType(AM_MEDIA_TYPE* mt);
  std::string mediaFormatMajorTypeToString(GUID type);
  std::string mediaFormatSubTypeToString(GUID type);
  std::string mediaFormatFormatTypeToString(GUID type);

 private:
  IGraphBuilder* graph;
  ICaptureGraphBuilder2* builder;
  IBaseFilter* capture_device_filter; // represents the found capture device
  IMediaControl* media_control; // used to control the event
  IBaseFilter* sample_grabber_filter; // used to retrieve sample in our callback
  ISampleGrabber* sample_grabber_iface; // necessary object to retrieve samples
  VideoCaptureDirectShowCallback* sample_grabber_listener; // the implementation 
  IBaseFilter* null_renderer_filter; // end point of our graph

  bool is_graph_setup;
  static int num_com_init;
  std::vector<VideoCaptureDirectShowDevice> devices;
};


// -------------------------------------------
inline STDMETHODIMP_(ULONG) VideoCaptureDirectShowCallback::AddRef() {
  return 1;
}

inline STDMETHODIMP_(ULONG) VideoCaptureDirectShowCallback::Release() {
  return 2;
}

inline STDMETHODIMP VideoCaptureDirectShowCallback::QueryInterface(REFIID riid, void **obj) {
  *obj = static_cast<ISampleGrabberCB*>(this);
  return S_OK;
}


#endif
