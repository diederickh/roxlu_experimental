#include <videorecorder/VideoIOH264.h>

VideoIOH264::VideoIOH264()
  :fp(NULL)
{
}

int VideoIOH264::getType() {
  return VIDEO_IO_H264;
}

int VideoIOH264::writeOpenFile(VideoParams* p) {
  fp = fopen(p->filepath.c_str(), "w+b");
  if(!fp) {
    printf("Error: Cannot open raw file.\n");
    return -1;
  }
  return 1;
}

int VideoIOH264::writeParams(VideoParams* p) {
  x264_nal_t* n = p->x264_nal;
  int size = n[0].i_payload + n[1].i_payload + n[2].i_payload;
  if(fwrite(n[0].p_payload, size, 1, fp)) {
    return size;
  }
  return 1;
}

int VideoIOH264::writeHeaders(VideoParams* p) {
  return -1;
}

int VideoIOH264::writeVideoPacket(VideoPacket* pkt) {
  if(fwrite(pkt->data, pkt->data_size, 1, fp)) {
    return pkt->data_size;
  }
  return -1;
}

int VideoIOH264::writeAudioPacket(AudioPacket* pkt) {
  return -1;
}

int VideoIOH264::writeCloseFile(VideoParams* p) {
  if(fp) {
    fclose(fp);
    fp = NULL;
  }
  return 0;
}

