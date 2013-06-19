/*

  # AudioStream

  The AudioStream class is used to read audio for playback.
  You add audio streams to the AudioOutput object. There are 
  two important AudioStream types: AudioStreamFile and AudioStreamMemory

  Use an AudioStreamFile when you want to playback large, multi minute 
  audio files and not many of these at the same time. 

  Use an AudioStreamMemory when you want to playback smaller sounds with
  a duration < 60 sec. These are typical for sound effects.

 */

#ifndef ROXLU_AUDIO_STREAM_H
#define ROXLU_AUDIO_STREAM_H

#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <audio/AudioFile.h>

#define AS_STATE_NONE 0
#define AS_STATE_PLAYING 1
#define AS_STATE_PAUSED 2

// ------------------------------------------------------------------

class AudioStream {
 public:
  AudioStream();
  virtual ~AudioStream();
  virtual size_t read(void** output, unsigned long nframes) = 0;
  virtual int getNumChannels() = 0;
  virtual int getFormat() = 0;
  virtual int getSampleRate() = 0;
  virtual void gotoFrame(int frame) = 0;                                            /* set the read index back to the start */
  
  void setState(int state);
  int getState();

 public:
  int state;
};

inline void AudioStream::setState(int st) {
  state = st;
}

inline int AudioStream::getState() {
  return state;
}

// ------------------------------------------------------------------

class AudioStreamFile : public AudioStream {
 public:
  AudioStreamFile();
  ~AudioStreamFile();
  bool load(std::string filename, bool datapath = false);
  size_t read(void** input, unsigned long nframes);
  int getNumChannels();
  int getFormat();
  int getSampleRate();
  void gotoFrame(int frame);
 public:
  AudioFile audio_file;
};

inline int AudioStreamFile::getNumChannels() {
  return audio_file.getNumChannels();
}

inline int AudioStreamFile::getFormat() {
  return audio_file.getFormat();
}

inline int AudioStreamFile::getSampleRate() {
  return audio_file.getSampleRate();
}

// ------------------------------------------------------------------

class AudioStreamMemory : public AudioStream {
 public:
  AudioStreamMemory();
  ~AudioStreamMemory();
  bool load(std::string filename, bool datapath = false);
  size_t read(void** input, unsigned long nframes);
  int getNumChannels();
  int getFormat();
  int getSampleRate();
  void gotoFrame(int frame);
 public:
  std::vector<short> buffer;
  size_t index;
  int num_channels;
  int format;
  int samplerate;
};

inline int AudioStreamMemory::getNumChannels() {
  return num_channels;
}

inline int AudioStreamMemory::getFormat() {
  return format;
}

inline int AudioStreamMemory::getSampleRate() {
  return samplerate;
}

inline void AudioStreamMemory::gotoFrame(int frame) {
  index = frame * num_channels;
}

#endif
