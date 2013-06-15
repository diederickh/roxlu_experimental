#ifndef ROXLU_AUDIO_PLAYER_H
#define ROXLU_AUDIO_PLAYER_H

#include <string>
#include <map>
#include <vector>
#include <audio/Audio.h>
#include <audio/AudioFile.h>
#include <roxlu/core/Log.h>
#include <sndfile.h>

#define AUDIO_PLAYER_STATE_NONE 0
#define AUDIO_PLAYER_STATE_PLAYING 1

#define AUDIO_PLAYER_EVENT_STARTED 1
#define AUDIO_PLAYER_EVENT_STOPPED 2

// ----------------------------------------------------------------------
class AudioPlayer; 

typedef void(*audio_player_callback)(AudioPlayer* player, int event, void* user);

// ----------------------------------------------------------------------

void audio_player_audio_out_callback(void* output, unsigned long nframes, void* user);

// ----------------------------------------------------------------------

class AudioPlayer {
 public:
  AudioPlayer();
  ~AudioPlayer();

  bool load(std::string file, bool datapath = false, audio_player_callback cbPlayer = NULL, void* cbUser = NULL);
  //bool openDefaultDevice();                                               /* opens the default output device; you can only open an specific output device once; a bit like that you can only open the same video capture device once */
  // bool addFile(int name, std::string filename, bool datapath = false);
  bool play();
  bool stop();

  PaSampleFormat libSndfileFormatToPortAudioFormat(int fmt);
  size_t getTotalReadFrames();
  int getSampleRate();

 public:
  Audio audio;
  vector<AudioFile*> audio_files;
  std::map<int, std::string> files;
  
  size_t total_read_frames;
  int samplerate;
  AudioFile audio_file;
  int state;
  audio_player_callback cb_player;
  void* cb_user;
  
};

// ----------------------------------------------------------------------

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
