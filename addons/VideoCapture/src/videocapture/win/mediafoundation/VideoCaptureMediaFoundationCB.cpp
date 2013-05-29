#include <videocapture/win/mediafoundation/VideoCaptureMediaFoundation.h>
#include <videocapture/win/mediafoundation/VideoCaptureMediaFoundationCB.h>
#include <assert.h>
#include <mfidl.h>
#include <shlwapi.h>

bool VideoCaptureMediaFoundationCB::createInstance(VideoCaptureMediaFoundation* cap, VideoCaptureMediaFoundationCB** cb) {

  if(cb == NULL) {
    RX_ERROR("The given VideoCaptureMediaFoundation is invalid; cant create an instance");
    return false;
  }

  VideoCaptureMediaFoundationCB* media_cb = new VideoCaptureMediaFoundationCB(cap);
  if(!media_cb) {
    RX_ERROR("Cannot allocate a VideoCaptureMediaFoundationCB object - out of memory");
    return false;
  }
  
  *cb = media_cb;
  (*cb)->AddRef();

  safeReleaseMediaFoundation(&media_cb); 
  return true;
}

VideoCaptureMediaFoundationCB::VideoCaptureMediaFoundationCB(VideoCaptureMediaFoundation* cap) 
  :ref_count(1)
  ,cap(cap)
{
  InitializeCriticalSection(&crit_sec);
}

VideoCaptureMediaFoundationCB::~VideoCaptureMediaFoundationCB() {
}

HRESULT VideoCaptureMediaFoundationCB::QueryInterface(REFIID iid, void** v) {
  static const QITAB qit[] = {
    QITABENT(VideoCaptureMediaFoundationCB, IMFSourceReaderCallback), { 0 },
  };
  return QISearch(this, qit, iid, v);
}

ULONG VideoCaptureMediaFoundationCB::AddRef() {
  return InterlockedIncrement(&ref_count);
}

ULONG VideoCaptureMediaFoundationCB::Release() {
  ULONG ucount = InterlockedDecrement(&ref_count);
  if(ucount == 0) {
    delete this;
  }
  return ucount;
}

HRESULT VideoCaptureMediaFoundationCB::OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp, IMFSample* sample) {
  assert(cap);
  assert(cap->imf_source_reader);
  assert(cap->cb_frame);

  EnterCriticalSection(&crit_sec);

  if(SUCCEEDED(hr) && sample) {

    IMFMediaBuffer* buffer;
    HRESULT hr = S_OK;
    DWORD count = 0;
    sample->GetBufferCount(&count);

    for(DWORD i = 0; i < count; ++i) {
      hr = sample->GetBufferByIndex(i, &buffer);
      if(SUCCEEDED(hr)) {
       
        DWORD length = 0;
        DWORD max_length = 0;
        BYTE* data = NULL;
        buffer->Lock(&data, &max_length, &length);
        cap->cb_frame((void*)data, length, cap->cb_user);
        buffer->Unlock();
        buffer->Release();
      }
    }
    
  }
  if(SUCCEEDED(hr)) {
    if(cap->imf_source_reader && cap->is_capturing) {
       hr = cap->imf_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
       if(FAILED(hr)) {
         RX_ERROR("Error while trying to read the next sample");
       }
    }
  }

  LeaveCriticalSection(&crit_sec);
  return S_OK;
}

HRESULT VideoCaptureMediaFoundationCB::OnEvent(DWORD, IMFMediaEvent* event) {
  return S_OK;
}

HRESULT VideoCaptureMediaFoundationCB::OnFlush(DWORD) {
  return S_OK;
}



