#ifndef ROXLU_PCMWRITERH
#define ROXLU_PCMWRITERH

#include <audio/AudioListener.h>
#include <roxlu/Roxlu.h>
#include <string>

class PCMWriter : public AudioListener {
 public:
  PCMWriter();
  ~PCMWriter();
  bool open(const std::string filepath);
  void onAudioIn(const void* input, unsigned long numFrames);
  void close();
 private:
  RingBuffer ring_buffer;
  FILE* fp;
};

#endif
