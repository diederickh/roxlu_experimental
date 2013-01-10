#ifndef ROXLU_VORBIS_WRITER_H
#define ROXLU_VORBIS_WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vorbis/vorbisenc.h>

/*

Record raw audio into ogg using libvorbis.

Example:
--------
VorbisWriter vorb;
vorb.open(File::toDataPath("test.ogg"), 44100, num_channels, sizeof(short), VW_INT16);

// then in your audio input callback:
vorb.onAudioIn(input, nframes)

 */

enum VorbisWriterFormat {
  VW_FLOAT32,
  VW_INT16
};

class VorbisWriter {
 public:
  VorbisWriter();
  ~VorbisWriter();
  void open(const std::string filepath, int samplerate, int nchannels, size_t bytesPerSample, VorbisWriterFormat f);
  void close();
  void onAudioIn(const void* input, unsigned long nframes);
 private:
  bool is_setup;
  int num_channels;
  int samplerate;
  size_t bytes_per_sample; // e.g. sizeof(float), sizeof(short)
  VorbisWriterFormat format;

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
