#ifndef ROXLU_CAPTURE_BUFFER_H
#define ROXLU_CAPTURE_BUFFER_H

#include <roxlu/Roxlu.h>
#include <ogg/OggMaker.h>
#include <vector>

struct CaptureFrame {
  char type; // V = video, A = audio
  rx_uint64 timestamp;
  size_t offset; // offset in file.
  size_t num_frames; // for audio
  size_t nbytes;
};

struct CaptureFrameSorter {
  bool operator()(const CaptureFrame& a, const CaptureFrame& b) {
    return a.timestamp < b.timestamp;
  }
};

class CaptureBuffer {
 public:
  CaptureBuffer();
  ~CaptureBuffer();
  void addAudioFrame(rx_uint64 timestamp, const void* data, size_t nbytes, size_t nframes);
  void addVideoFrame(rx_uint64 timestamp, const void* data, size_t nbytes);
  void encode();
 public:
  bool record;
  bool got_audio;
  bool got_video;
  FILE* audio_fp;
  FILE* video_fp;
  size_t naudio_frames;
  size_t nvideo_frames;
  std::vector<CaptureFrame> frames;
  size_t video_bytes_written;
  size_t audio_bytes_written;
  size_t audio_samples_written;
  int audio_samplerate; // 16000, 44100 how many frames per second
  int fps;
  int millis_per_frame;
  int image_w;
  int image_h;
  int num_channels;// number of audio channels
  size_t bytes_per_image;
  size_t bytes_per_audio;
  unsigned char* tmp_video_buffer;
  unsigned char* tmp_audio_buffer;
  OggMaker ogg_maker;
};
#endif
