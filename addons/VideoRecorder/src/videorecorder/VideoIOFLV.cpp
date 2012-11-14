#include <videorecorder/VideoIOFLV.h>

int VideoIOFLV::getType() {
  return VIDEO_IO_FLV;
}

int VideoIOFLV::writeOpenFile(VideoParams* p) {
  filepath = p->filepath;
  int s = flv.writeHeader(true, true);
  flush(flv.getBuffer());
  return s;
}

int VideoIOFLV::writeParams(VideoParams* p) {
  int s = flv.writeParams(p);
  flush(flv.getBuffer());
  return s;
}

int VideoIOFLV::writeHeaders(VideoParams* p) {
  int s = flv.writeHeaders(p);
  flush(flv.getBuffer());
  return s;
}

int VideoIOFLV::writeVideoPacket(VideoPacket* pkt) {
  int s = flv.writeVideoPacket(pkt);
  flush(flv.getBuffer());
  return s;
}

int VideoIOFLV::writeAudioPacket(AudioPacket* pkt) {
  int s = flv.writeAudioPacket(pkt);
  flush(flv.getBuffer());
  return s;
}

int VideoIOFLV::writeCloseFile(VideoParams* p) {
  flv.saveFile(filepath.c_str());
  return 0;
}

void VideoIOFLV::flush(Buffer& buffer) {
}
