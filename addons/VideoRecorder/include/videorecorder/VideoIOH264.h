#ifndef ROXLU_VIDEORECORDER_VIDEOIOH264
#define ROXLU_VIDEORECORDER_VIDEOIOH264

#include <string>
#include <videorecorder/VideoIO.h>
#include <videorecorder/VideoTypes.h>
#include <roxlu/Roxlu.h>

class VideoIOH264 : public VideoIO {
 public:
  VideoIOH264(const std::string filepath);
  int writeOpenFile(VideoParams* p);
  int writeParams(VideoParams* p);
  int writeHeaders(VideoParams* p);
  int writeVideoFrame(VideoParams* p);
  int writeAudioFrame(VideoParams* p);
  int writeCloseFile(VideoParams* p);
	
  // --------
  int writeVideoPacket(VideoPacket* p); // @todo to be removed
  int writeAudioPacket(AudioPacket* p); // @todo to be removed
  // --------

  virtual void flush(Buffer& buffer); // for VideoIOH264Socket
 private:
  std::string filepath;
  FILE* fp;
};

#endif



