#ifndef ROXLU_VIDEORECORDER_VIDEO_IOH
#define ROXLU_VIDEORECORDER_VIDEO_IOH

#include <roxlu/Roxlu.h>
#include <videorecorder/VideoTypes.h>

extern "C" {
#include <x264.h>
}

class VideoIO {
 public:
  virtual int writeOpenFile(VideoParams* p) = 0;
  virtual int writeParams(VideoParams* p) = 0;
  virtual int writeHeaders(VideoParams* p) = 0;
  virtual int writeCloseFile(VideoParams* p) = 0;
  virtual int writeVideoPacket(VideoPacket* pkt) = 0;
  virtual int writeAudioPacket(AudioPacket* pkt) = 0;
};

#endif




