#ifndef ROXLU_VIDEOCAPTURE_DIRECTSHOW_CB_H
#define ROXLU_VIDEOCAPTURE_DIRECTSHOW_CB_H

#include <stdio.h>
#include <windows.h>
#include <dshow.h>
#include <algorithm>
#include <vector>
#include <string>
#include <videocapture/win/directshow/qedit.h>
#include <videocapture/Types.h>
#include <videocapture/VideoCaptureBase.h>

class VideoCaptureDirectShow2;

class VideoCaptureDirectShowCB : public ISampleGrabberCB {
 public:
  VideoCaptureDirectShowCB(VideoCaptureDirectShow2* cap);
  ~VideoCaptureDirectShowCB();
  HRESULT STDMETHODCALLTYPE SampleCB(double timestamp, IMediaSample* sample);      /* non buffered i/o */
  HRESULT STDMETHODCALLTYPE BufferCB(double timestamp, BYTE* buffer, long size);   /* buffered i/o */
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  STDMETHODIMP QueryInterface(REFIID riid, void** refD);

 private:
  VideoCaptureDirectShow2* cap;
  CRITICAL_SECTION critical_section;

};

#endif
