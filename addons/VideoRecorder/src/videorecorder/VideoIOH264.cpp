#include <videorecorder/VideoIOH264.h>

VideoIOH264::VideoIOH264(const std::string filepath)
  :filepath(filepath)
  ,fp(NULL)
{
}

int VideoIOH264::writeOpenFile(VideoParams* p) {
  printf("> OpenFile.\n");


  // int s = flv.writeHeader(true, true);
  // flush(flv.getBuffer());
  fp = fopen(filepath.c_str(), "w+b");
  if(!fp) {
    printf("Error: Cannot open raw file.\n");
    return -1;
  }

  return 1;
}

int VideoIOH264::writeParams(VideoParams* p) {
  
  //  int s = flv.writeParams(p);
  // flush(flv.getBuffer());
  printf("> Write headers.\n");
  x264_nal_t* n = p->x264_nal;
  printf("%p, %p, %d\n", n, fp, n[1].i_payload);
  //  return 1;
  int size = n[0].i_payload + n[1].i_payload + n[2].i_payload;

  if(fwrite(n[0].p_payload, size, 1, fp)) {
    printf("WRITTEN: %d\n", size);
    return size;
  }


  return 1;
}

int VideoIOH264::writeHeaders(VideoParams* p) {

  // int s = flv.writeHeaders(p);
  // flush(flv.getBuffer());
  return -1;
}

// we use writeVideoPacket(), remove this one
int VideoIOH264::writeVideoFrame(VideoParams* p) {
  //	int s = flv.writeVideoFrame(p);
  //flush(flv.getBuffer());
  //return s;
  return -1;
}

int VideoIOH264::writeVideoPacket(VideoPacket* pkt) {
  printf("> Write video frame: %p., size: %zu\n", fp, pkt->data_size);
  // int s = flv.writeVideoPacket(pkt);
  // flush(flv.getBuffer());
  if(fwrite(pkt->data, pkt->data_size, 1, fp)) {
    
    return pkt->data_size;
  }

  return -1;
}

int VideoIOH264::writeAudioPacket(AudioPacket* pkt) {
  // int s = flv.writeAudioPacket(pkt);
  // flush(flv.getBuffer());
  return -1;
}


// we use writeAudioPacket (remove this one)
int VideoIOH264::writeAudioFrame(VideoParams* p) {
  //	int s = flv.writeAudioFrame(p);
  //	flush(flv.getBuffer());
  //	return s;
  return -1;
}

int VideoIOH264::writeCloseFile(VideoParams* p) {
  //  flv.saveFile("data/particles_flv2.flv");
  printf("> cose\n");
  if(fp) {
    fclose(fp);
    fp = NULL;
  }

  return 0;
}


void VideoIOH264::flush(Buffer& buffer) {
}
