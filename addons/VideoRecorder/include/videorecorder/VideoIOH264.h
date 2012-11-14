#ifndef ROXLU_VIDEORECORDER_VIDEOIOH264
#define ROXLU_VIDEORECORDER_VIDEOIOH264

#include <string>
#include <videorecorder/VideoIO.h>
#include <videorecorder/VideoTypes.h>
#include <roxlu/Roxlu.h>

/*

  Encode directly into a raw h264 file.

  Convert to some other format:
  ./avconv -i io_output.264 -vcodec copy out2.mov

 */

class VideoIOH264 : public VideoIO {
 public:
  VideoIOH264();
  int writeOpenFile(VideoParams* p);
  int writeParams(VideoParams* p);
  int writeHeaders(VideoParams* p);
  int writeCloseFile(VideoParams* p);
  int writeVideoPacket(VideoPacket* p);
  int writeAudioPacket(AudioPacket* p);
 private:
  std::string filepath;
  FILE* fp;
};

#endif



