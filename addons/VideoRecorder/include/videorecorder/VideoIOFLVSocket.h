#ifndef ROXLU_VIDEORECORDER_VIDEOIOFLV_SOCKETH
#define ROXLU_VIDEORECORDER_VIDEOIOFLV_SOCKETH

#include <videorecorder/VideoIO.h>
#include <videorecorder/VideoIOFLV.h>
#include <videorecorder/FLV.h>
#include <roxlu/Roxlu.h>

class VideoIOFLVSocket : public VideoIOFLV {
 public:
  void connect(const char* ip, unsigned short int port);
  void flush(Buffer& buffer);
 private:
  Socket sock;
};

#endif



