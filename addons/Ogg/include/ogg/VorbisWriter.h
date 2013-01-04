#ifndef ROXLU_VORBIS_WRITER_H
#define ROXLU_VORBIS_WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vorbis/vorbisenc.h>

class VorbisWriter {
 public:
  VorbisWriter();
  ~VorbisWriter();
  void open(const std::string filepath);
  void close();
  void onAudioIn(const void* input, unsigned long nframes);
 private:
  bool is_setup;
  vorbis_info vi;
  vorbis_comment vc;
  vorbis_dsp_state vd;
  vorbis_block vb;
  ogg_stream_state os;
  ogg_page og;
  ogg_packet op;

  FILE* fp;
};
#endif
