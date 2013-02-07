#ifndef ROXLU_FLV_SCREENRECORDER_H
#define ROXLU_FLV_SCREENRECORDER_H

#include <inttypes.h>
#include <flv/AV.h>
#include <flv/FLV.h>
#include <roxlu/Roxlu.h>
#include <string>

//#define SCREEN_RECORDER_USE_PBO
#define SCREEN_RECORDER_NUM_PBOS 5

struct FLVScreenRecorderSettings {
  FLVScreenRecorderSettings();
  ~FLVScreenRecorderSettings();
  void reset(); 

  /* video settings */
  unsigned int vid_in_w;
  unsigned int vid_in_h;
  unsigned int vid_out_w;
  unsigned int vid_out_h;
  int vid_fps;

  /* audio settings */
  unsigned int audio_num_channels;
  unsigned int audio_samplerate;
  unsigned int audio_max_samples;
  AVAudioFormat audio_format;
};

class FLVScreenRecorder {
 public:
  FLVScreenRecorder();
  ~FLVScreenRecorder();

  bool setup(FLVScreenRecorderSettings settings);

  bool open(std::string filepath);
  
  void grabFrame();
  void addAudio(void* input, int nframes);

  void start(); // must be called once, starts the encoding thread
  void stop(); // shuts down the encoder, is automatically called by destructor too.

 private:
  FLVScreenRecorderSettings settings;

  /* Recording */
  AV av;
  FLVFileWriter flv_writer;
  FLV flv;
  bool is_recording;
  bool is_file_opened;

  /* GL */
  unsigned int channels;

  GLuint pbos[SCREEN_RECORDER_NUM_PBOS];
  unsigned int dx;
#if !defined(SCREEN_RECORDER_USE_PBO)
  unsigned char* pixels;
#endif
};
#endif
