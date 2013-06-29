/*

  # AudioStream

  The AudioStream class is used to read audio for playback.
  You add audio streams to the AudioOutput object. There are 
  two important AudioStream types: AudioStreamFile and AudioStreamMemory

  Use an AudioStreamFile when you want to playback large, multi minute 
  audio files and not many of these at the same time. AudioStreamFile
  needs some more work - still very experimental code - 

  Use an AudioStreamMemory when you want to playback smaller sounds with
  a duration < 60 sec. These are typical for sound effects.

 */

#ifndef ROXLU_AUDIO_STREAM_H
#define ROXLU_AUDIO_STREAM_H

#include <assert.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <audio/AudioFile.h>

#define AST_STATE_NONE 0                                                             /* you should set the state of an output stream to `open` when it's opened and make check this in `accumulate()` and and `read()` */
#define AST_STATE_OPEN 1          

// ------------------------------------------------------------------

class AudioStream {
 public:
  AudioStream();                                                                    /* the AudioStream is used for reading from audio streams. this is used by the AudioOutput to read frames */
  virtual ~AudioStream();
  virtual size_t read(void* output, unsigned long nframes) = 0;                     /* should read nframes into output, overwriting the data in output */
  virtual size_t accumulate(void* output, unsigned long nframes) = 0;               /* should accumulate data to the output */
  virtual int getNumChannels() = 0;                                                 /* return the number of channels */
  virtual int getFormat() = 0;                                                      /* get the format using libsndfile format codes */
  virtual int getSampleRate() = 0;                                                  /* get the samplerate, using libsndfile samplerate codes */
  virtual void gotoFrame(int frame) = 0;                                            /* set the read index back to the start */
 public:
  int state;
};

// ------------------------------------------------------------------

class AudioStreamFile : public AudioStream {
 public:
  AudioStreamFile();
  ~AudioStreamFile();
  bool load(std::string filename, bool datapath = false);
  size_t read(void* output, unsigned long nframes);
  size_t accumulate(void* output, unsigned long nframes);
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
  size_t read(void* output, unsigned long nframes);
  size_t accumulate(void* output, unsigned long nframes);
  int getNumChannels();
  int getFormat();
  int getSampleRate();
  void gotoFrame(int frame);
 public:
  std::vector<float> buffer;
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
