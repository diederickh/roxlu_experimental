#ifndef ROXLU_PORTAUDIO_LISTENER_H
#define ROXLU_PORTAUDIO_LISTENER_H

class AudioListener {
 public:
  virtual void onAudioIn(const void* input, unsigned long numFrames) = 0;
};
#endif
