#ifndef ROXLU_MP3_WRITER_H
#define ROXLU_MP3_WRITER_H

#include <roxlu/core/Log.h>
#include <lame/lame.h>
#include <string>
#include <fstream>

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
#define MP3_WRERR_CANNOT_FLUSH "Something went wrong while flushing the mp3 buffer."
#define MP3_WRERR_ID3_TRACK_OOR "The given id3 track number is out of range"
#define MP3_WRERR_ID3_TRACK_INVALID "The given id3 track number is invalid"
#define MP3_WRERR_FILE_OPEN "Cannot open the given file: `%s`"

typedef void(*mp3_writer_open_callback)(void* user);
typedef void(*mp3_writer_data_callback)(const char* data, size_t nbytes, void* user);
typedef void(*mp3_writer_close_callback)(void* user);


void mp3_writer_default_open(void* user);                                   /* default writer functions: opens a file */
void mp3_writer_default_data(const char data, size_t nbytes, void* user);   /* default writer functions: writes the given data to the file */
void mp3_writer_default_close(void* user);                                  /* default writer functions: closes a file */

struct MP3FileWriter {
  MP3FileWriter(std::string filename, bool datapath = false);            /* opens the file in the given path */
  ~MP3FileWriter();

  std::ofstream ofs;
  mp3_writer_open_callback cb_open;                                      /* default open function, is set to mp3_writer_default_open() */
  mp3_writer_data_callback cb_data;                                      /* default data function, is set to mp3_writer_default_data() */
  mp3_writer_close_callback cb_close;                                    /* default close function, is set toe mp3_writer_default_close() */

};

enum MP3WriterSampleRate {
  MP3_WR_SAMPLERATE_16000,
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
  int samplerateToInt();
  bool hasID3();                                                          /* returns true if one of the id3_* field has a value */

  int bitrate;                                                            /* the bitrate in kilo bits */
  int num_channels;                                                       /* number of channels in the incoming audio stream */
  int quality;                                                            /* the quality to use. value between 0 and 9, 0 = best (slow), 9 = worst (fast). Value around 5 is suited for most situations */
  MP3WriterSampleRate samplerate;                                         /* the sample rate */ 
  MP3WriterMode mode;                                                     /* what mode to use: mono, stereo modes */
  mp3_writer_open_callback cb_open;                                       /* the mp3 writer only encodes audio and passes state changes into the callbacks */
  mp3_writer_data_callback cb_data;                                       /* gets called when new data is received */
  mp3_writer_close_callback cb_close;                                     /* gets called when we close the mp3 stream */
  std::string id3_title;                                                  /* id3 tag: title of the song */
  std::string id3_artist;                                                 /* id3 tag: name of the artist */
  std::string id3_album;                                                  /* id3 tag: album of the song */
  std::string id3_year;                                                   /* id3 tag: year of recording */
  std::string id3_comment;                                                /* id3 tag: additional comments */
  std::string id3_track;                                                  /* id3 tag: track number */
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
  void addAudioMono(const short int* data, size_t nbytes, int nsamples);            /* add mono 16bits short int audio */

 public:
  MP3WriterConfig config;                                                           /* the configuration we use to open the stream, write data to the stream and close the stream. is also used to setup the encoder */

 private:
  bool is_started;
  bool is_setup;
  lame_global_flags* lame_flags;
  unsigned char mp3_buffer[MP3_WRITER_BUFFER_SIZE];
};

#endif
