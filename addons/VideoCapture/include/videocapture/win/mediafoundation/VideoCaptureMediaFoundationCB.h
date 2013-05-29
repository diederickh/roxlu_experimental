#ifndef ROXLU_VIDEO_CAPTURE_MEDIA_FOUNDATION_CALLBACK_H
#define ROXLU_VIDEO_CAPTURE_MEDIA_FOUNDATION_CALLBACK_H

#include <windows.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <roxlu/core/Log.h>

class VideoCaptureMediaFoundation;
class IMFSourceReaderCallback;    
                                         
class VideoCaptureMediaFoundationCB : public IMFSourceReaderCallback {
 public:
  static bool createInstance(VideoCaptureMediaFoundation* cap, VideoCaptureMediaFoundationCB** cb);

  STDMETHODIMP QueryInterface(REFIID iid, void** v);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();
  
  STDMETHODIMP OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp, IMFSample* sample);
  STDMETHODIMP OnEvent(DWORD streamIndex, IMFMediaEvent* event);
  STDMETHODIMP OnFlush(DWORD streamIndex);

  HRESULT Wait(DWORD* streamFlags, LONGLONG* timestamp, IMFSample* sample) { return S_OK;  }
  HRESULT Cancel() { return S_OK; } 

 private:
  VideoCaptureMediaFoundationCB(VideoCaptureMediaFoundation* cap);
  virtual ~VideoCaptureMediaFoundationCB();

 private:
  VideoCaptureMediaFoundation* cap;
  long ref_count;
  CRITICAL_SECTION crit_sec;
  
};

// ----------------------------------------------


#endif
