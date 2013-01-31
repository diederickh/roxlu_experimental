#ifndef ROXLU_FLV_SCREENRECORDER_H
#define ROXLU_FLV_SCREENRECORDER_H

#include <inttypes.h>
#include <flv/AV.h>
#include <flv/FLV.h>
#include <roxlu/Roxlu.h>
#include <string>

#define SCREEN_RECORDER_USE_PBO
#define SCREEN_RECORDER_NUM_PBOS 5

class ScreenRecorder {
 public:
  ScreenRecorder();
  ~ScreenRecorder();
  bool setup(std::string filepath, 
    unsigned int inWidth, 
    unsigned int inHeight,
    unsigned int outWdth,
    unsigned int outHeight,
    int fps);
  
  void grabFrame();

  void start(); // must be called once, starts the encoding thread
  void stop(); // shuts down the encoder, is automatically called by destructor too.

 private:

  /* Recording */
  AV av;
  FLVFileWriter flv_writer;
  FLV flv;
  bool is_recording;

  /* GL */
  unsigned int width;
  unsigned int height;
  unsigned int channels;
  unsigned char* pixels;
  GLuint pbos[SCREEN_RECORDER_NUM_PBOS];
  unsigned int dx;
};
#endif
