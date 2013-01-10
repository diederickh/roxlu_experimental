#ifndef ROXLU_VIDEOCAPTURE_DIRECTSHOW_H
#define ROXLU_VIDEOCAPTURE_DIRECTSHOW_H

#pragma warning(once : 4995)
//#define _ATL_NO_UUIDOF 

#include <stdio.h>
#include <windows.h>
#include <dshow.h>
#include <vector>
#include <string>
#include <videocapture/qedit.h>
#include <videocapture/rx_capture.h>
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
 * - ISampleGrabberCB implementation:
 *    http://msdn.microsoft.com/en-us/library/windows/desktop/dd376985(v=vs.85).aspx
 */

#define LOG(x) printf x
#define RETURN_IF_FAILED(hr, msg, rval) { if(FAILED(hr)) { printf(msg); return rval; } }

class VideoCaptureDirectShow;
class VideoCaptureDirectShowCallback : public ISampleGrabberCB {
 public:
  VideoCaptureDirectShowCallback(VideoCaptureDirectShow* vidcap);
  ~VideoCaptureDirectShowCallback();
  HRESULT STDMETHODCALLTYPE SampleCB(double timestamp, IMediaSample* sample); // non buffered i/o
  HRESULT STDMETHODCALLTYPE BufferCB(double timestamp, BYTE* buffer, long size); // buffered i/o
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  STDMETHODIMP QueryInterface(REFIID riid, void** refD);
  bool new_frame;
 private:
  VideoCaptureDirectShow* vidcap;
  CRITICAL_SECTION critical_section;  
};

struct VideoCaptureDirectShowDevice {
  std::string name;
};

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd376992(v=vs.85).aspx
enum VideoCaptureDirectShowCallbackTypes {
   SAMPLED_CALLBACK = 0
  ,BUFFERED_CALLBACK = 1
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
  int getWidth();
  int getHeight();
  int setFrameCallback(rx_capture_frame_cb cb, void* user);
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
  int saveGraphToFile();
 public:
  rx_capture_frame_cb frame_cb;
  void* frame_user;
  VideoCaptureDirectShowCallback* sample_grabber_listener; // the implementation 
 private:
  int width;
  int height;
  IGraphBuilder* graph;
  ICaptureGraphBuilder2* builder;
  IBaseFilter* capture_device_filter; // represents the found capture device
  IMediaControl* media_control; // used to control the event
  IMediaEvent* media_event;
  IBaseFilter* sample_grabber_filter; // used to retrieve sample in our callback
  ISampleGrabber* sample_grabber_iface; // necessary object to retrieve samples

  IBaseFilter* null_renderer_filter; // end point of our graph

  bool is_graph_setup;
  static int num_com_init;
  std::vector<VideoCaptureDirectShowDevice> devices;
  int callback_type;  // 0: SampleCB (should be faster but crashes somehow), 1: BufferCB

  friend class VideoCaptureDirectShowCallback;
};

inline int VideoCaptureDirectShow::getWidth() {
  return width;
}

inline int VideoCaptureDirectShow::getHeight() {
  return height;
}

//inline int VideoCaptureDirectShow::setFrameCallback(videocapture_directshow_cb cb, void* user) {
inline int VideoCaptureDirectShow::setFrameCallback(rx_capture_frame_cb cb, void* user) {
  frame_cb = cb;
  frame_user = user;
  return 1;
}


struct rx_capture_t {
  VideoCaptureDirectShow* cap;
};

#endif
