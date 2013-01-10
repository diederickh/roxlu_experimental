#ifndef ROXLU_PCMWRITERH
#define ROXLU_PCMWRITERH

#include <roxlu/Roxlu.h>
#include <string>

class PCMWriter {
 public:
  PCMWriter();
  ~PCMWriter();
  bool open(const std::string filepath, int expectedSecondsToRecord, int samplerate, int nchannels, size_t bytesPerSample);
  void onAudioIn(const void* input, unsigned long numFrames);
  void close();
 private:
  RingBuffer ring_buffer;
  FILE* fp;
  size_t bytes_per_sample; // e.g. sizeof(short) for 16bit, sizeof(float) 32bit
  int num_channels;  
  int samplerate;
  int expected_seconds_to_record;
};

#endif
