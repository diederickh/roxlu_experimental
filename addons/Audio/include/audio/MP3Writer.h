#ifndef ROXLU_MP3_WRITER_H
#define ROXLU_MP3_WRITER_H

#include <roxlu/core/Log.h>
#include <lame/lame.h>

#define MP3_WRITER_BUFFER_SIZE 8192
#define MP3_WRERR_NOT_SETUP "Cannot begin with encoding MP3 as we havent been setup. Call setup() first"
#define MP3_WRERR_NOT_STARTED "Cannot end() because we've not yet started a stream"
#define MP3_WRERR_MODE_NOT_SET "Cannot validate because the mode hasnt been set. Please set a mode for the config"
#define MP3_WRERR_SAMPLERATE_NOT_SET "Cannot validate the configuration because the sample rate hasnt been set. "
#define MP3_WRERR_WRONG_NCHANNELS "We only support 1 or 2 channels"
#define MP3_WRERR_NO_OPEN_CB "No mp3_writer_open_callback set! Please add it to the config"
#define MP3_WRERR_NO_DATA_CB "No mp3_writer_data_callback set! Please add it to the config"
#define MP3_WRERR_NO_CLOSE_CB "No mp3_writer_close_callback set! Please add it to the config"
#define MP3_WRERR_NO_BITRATE "Cannot validate because not bitrate is set int the config. Use e.g. 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 192, 224, 256, 320"
#define MP3_WRERR_LAME_INIT "Cannot initialize lame"
#define MP3_WRERR_LAME_PARAMS "Cannot initialze parameters"

typedef void(*mp3_writer_open_callback)(void* user);
typedef void(*mp3_writer_data_callback)(const char* data, size_t nbytes, void* user);
typedef void(*mp3_writer_close_callback)(void* user);

enum MP3WriterSampleRate {
  MP3_WR_SAMPLERATE_44100,
  MP3_WR_SAMPLERATE_NOT_SET
};

enum MP3WriterMode {
  MP3_WR_MODE_MONO,
  MP3_WR_MODE_STEREO,
  MP3_WR_MODE_NOT_SET
};

struct MP3WriterConfig {
  MP3WriterConfig();
  ~MP3WriterConfig();
  void clear();
  bool validate();

  int bitrate;                                                            /* the bitrate in kilo bits */
  int num_channels;                                                       /* number of channels in the incoming audio stream */
  int quality;                                                            /* the quality to use. value between 0 and 9, 0 = best (slow), 9 = worst (fast). Value around 5 is suited for most situations */
  MP3WriterSampleRate samplerate;                                         /* the sample rate */ 
  MP3WriterMode mode;                                                     /* what mode to use: mono, stereo modes */
  mp3_writer_open_callback cb_open;                                       /* the mp3 writer only encodes audio and passes state changes into the callbacks */
  mp3_writer_data_callback cb_data;                                       /* gets called when new data is received */
  mp3_writer_close_callback cb_close;                                     /* gets called when we close the mp3 stream */
  void* user;                                                             /* gets passed into the callback functions */                     
};

class MP3Writer {
 public:
  MP3Writer();
  ~MP3Writer();

  bool setup(MP3WriterConfig cfg);                                                  /* gets passed into the mp3 stream */
  bool begin();                                                                     /* starts a new mp3 audio stream */
  bool end();                                                                       /* ends the current mp3 audio stream and makes sure everything is closed correctly, also frees all memory  */
  void addAudioInterleaved(const short int* data, size_t nbytes, int nsamples);     /* add interleaved 16bits short int audio */

 private:
  bool is_started;
  bool is_setup;
  lame_global_flags* lame_flags;
  unsigned char mp3_buffer[MP3_WRITER_BUFFER_SIZE];
  MP3WriterConfig config;                                                           /* the configuration we use to open the stream, write data to the stream and close the stream. is also used to setup the encoder */

};
#endif
