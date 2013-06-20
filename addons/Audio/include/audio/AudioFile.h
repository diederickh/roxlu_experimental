/*

  # AudioFile

  Wrapper around the awesome libsndfile library. 
  
  ## IMPORTANT:

  For now we always convert the audio data to `signed short`, though the getFormat() 
  return the format of the original sound file.

 */
#ifndef ROXLU_AUDIO_FILE
#define ROXLU_AUDIO_FILE

#include <roxlu/core/Utils.h>
#include <sndfile.h>
#include <audio/Audio.h>

void audio_file_audio_out_callback(void* output, unsigned long nframes, void* user);

class AudioFile {
 public:
  AudioFile();
  ~AudioFile();
  
  bool load(std::string filename, bool datapath = false);
  void print();
  sf_count_t readItems(void* output, sf_count_t nitems);                /* read nitems;  */
  sf_count_t readFrames(void* output, unsigned long nframes);           /* read nframes; */
  sf_count_t seek(sf_count_t frames, int whence);                       /* seek to a certain position (in frames). whence can be SEEK_SET, SEEK_CUR, SEEK_END */
  int getNumChannels();
  int getFormat();
  int getSampleRate();
  int getNumFramesPerChunk();
  void close();                                                          /* closes the file and frees memory */
 public:
  bool is_loaded;
  SF_INFO info;
  SNDFILE* handle;

  int num_frames;                                                        /* how many frames to read each chunk */ 
  size_t buffer_size;                                                    /* the size of our internal buffer in bytes */
  short* buffer;                                                         /* used to store temporary data @TODO: we need to create a function which simply returns some chunk of audio data */
};

#define AUDIOFILE_INIT_CHECK() \
  if(!handle) { \
    RX_ERROR("not yet initialized.");   \
    return 0; \
  } 


inline int AudioFile::getNumChannels() {
  AUDIOFILE_INIT_CHECK();
  return info.channels;
}

inline int AudioFile::getFormat() {
  AUDIOFILE_INIT_CHECK();
  return info.format;
}

inline int AudioFile::getSampleRate() {
  AUDIOFILE_INIT_CHECK();
  return info.samplerate;
}

inline int AudioFile::getNumFramesPerChunk() {
  AUDIOFILE_INIT_CHECK();
  return num_frames;
}

#endif
