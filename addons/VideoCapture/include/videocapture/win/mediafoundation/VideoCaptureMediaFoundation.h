#ifndef ROXLU_VIDEOCAPTURE_MEDIA_FOUNDATION_H
#define ROXLU_VIDEOCAPTURE_MEDIA_FOUNDATION_H

#include <assert.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <new>
#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <roxlu/core/Log.h>
#include <videocapture/VideoCaptureBase.h>
#include <videocapture/Types.h>
#include <videocapture/win/mediafoundation/VideoCaptureMediaFoundationCB.h>

// ----------------------------------------------

class VideoCaptureMediaFoundation : public VideoCaptureBase {
 public:
  VideoCaptureMediaFoundation();
  ~VideoCaptureMediaFoundation();
  
  /* Device control */
  int listDevices();
  bool openDevice(int device, VideoCaptureSettings cfg);
  bool closeDevice();
  bool startCapture();
  bool stopCapture();
  void update();

  /* Capabilities */
  std::vector<AVCapability> getCapabilities(int device);

 private:
  /* Utils + capabilites*/
  bool createSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader);
  bool createVideoDeviceSource(int device, IMFMediaSource** source);                                                   /* source will be set to the found device. */

  bool getCapabilities(IMFMediaSource* source, std::vector<AVCapability>& result);
  bool setDeviceFormat(IMFMediaSource* source, DWORD formatIndex);
  bool setReaderFormat(IMFSourceReader* reader, AVCapability cap);

  /* Debug + conversion */
  void printDeviceNames(IMFActivate** devices, UINT count);
  std::string mediaFoundationVideoFormatToString(const GUID& guid);
  AVPixelFormat mediaFoundationVideoFormatToLibavPixelFormat(GUID guid);
  GUID libavPixelFormatToMediaFoundationVideoFormat(AVPixelFormat fmt);

 public:
  VideoCaptureSettings settings;
  VideoCaptureMediaFoundationCB* mediaf_cb;
  IMFMediaSource* imf_media_source;
  IMFSourceReader* imf_source_reader;
  bool is_capturing;
};

// ----------------------------------------------
template<class T> void safeReleaseMediaFoundation(T** t);
#endif
