#ifndef ROXLU_AUDIO_PLAYER_H
#define ROXLU_AUDIO_PLAYER_H

#include <string>
#include <audio/Audio.h>
#include <audio/AudioFile.h>
#include <roxlu/core/Log.h>
#include <sndfile.h>

void audio_player_audio_out_callback(void* output, unsigned long nframes, void* user);

class AudioPlayer {
 public:
  AudioPlayer();
  ~AudioPlayer();

  bool load(std::string file, bool datapath = false);
  bool play();
  bool stop();

  PaSampleFormat libSndfileFormatToPortAudioFormat(int fmt);
  size_t getTotalReadFrames();
  int getSampleRate();

 public:
  size_t total_read_frames;
  int samplerate;
  Audio audio;
  AudioFile audio_file;
};

inline PaSampleFormat AudioPlayer::libSndfileFormatToPortAudioFormat(int fmt) {
  if(fmt & SF_FORMAT_PCM_S8) {
    return paInt8;
  }
  else if(fmt & SF_FORMAT_PCM_16) { 
     return paInt16;
  }
  else if(fmt & SF_FORMAT_PCM_24) {
    return paInt24;
  }
  else if(fmt & SF_FORMAT_PCM_32) {
    return paInt32;
  }
  else {
     RX_ERROR("Unhandled format");
     return 0;
  }
}

inline size_t AudioPlayer::getTotalReadFrames() {
  return total_read_frames;
}

inline int AudioPlayer::getSampleRate() {
  return samplerate;
}

#endif
