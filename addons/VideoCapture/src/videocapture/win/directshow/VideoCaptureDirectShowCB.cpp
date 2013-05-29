#include <roxlu/core/Log.h>
#include <assert.h>
#include <videocapture/win/directshow/VideoCaptureDirectShow.h>
#include <videocapture/win/directshow/VideoCaptureDirectShowCB.h>

VideoCaptureDirectShowCB::VideoCaptureDirectShowCB(VideoCaptureDirectShow2* cap)
  :cap(cap)
{
  InitializeCriticalSection(&critical_section);
}

VideoCaptureDirectShowCB::~VideoCaptureDirectShowCB() {
  DeleteCriticalSection(&critical_section);
}

HRESULT STDMETHODCALLTYPE VideoCaptureDirectShowCB::SampleCB(double timestamp, IMediaSample* sample) {
  return S_OK;
}

HRESULT STDMETHODCALLTYPE VideoCaptureDirectShowCB::BufferCB(double timestamp, BYTE* buffer, long size) {
  assert(cap);
  assert(cap->cb_frame);
  
  cap->cb_frame((void*)buffer, size, cap->cb_user);
  
  return S_OK;
}

STDMETHODIMP_(ULONG) VideoCaptureDirectShowCB::AddRef()  {
  return 1;
}

STDMETHODIMP_(ULONG) VideoCaptureDirectShowCB::Release()  {
  return 2;
}

STDMETHODIMP VideoCaptureDirectShowCB::QueryInterface(REFIID riid, void** obj)  {
  if((riid == IID_ISampleGrabberCB) || (riid == IID_IUnknown)) {
    *obj = static_cast<ISampleGrabberCB*>(this);
    return S_OK;
  }
  else {
    return E_NOINTERFACE;
  }
}







