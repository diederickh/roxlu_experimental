#ifndef ROXLU_WEBM_SCREENRECORDER_H
#define ROXLU_WEBM_SCREENRECORDER_H

//#define WEBM_SCREENREC_USE_PBOS 
#define WEBM_SCREENREC_NUM_PBOS

#include <string>
#include <webm/EBML.h>
#include <webm/Webm.h>
#include <roxlu/opengl/OpenGLInit.h>

extern "C" {
#  include <vpx/vpx_encoder.h>
}


struct WebmScreenRecorderSettings {
  WebmScreenRecorderSettings();
  ~WebmScreenRecorderSettings();
  bool use_video;                                          /* set to true if we need to record video */
  bool use_audio;                                          /* set to true if we need to record audio */
  ebml_write_cb cb_write;                                  /* callback: write ebml data */
  ebml_close_cb cb_close;                                  /* callback: close ebml stream */
  ebml_peek_cb cb_peek;                                    /* callback: peek one byte from stream */
  ebml_read_cb cb_read;                                    /* callback: read ebml data from stream */
  ebml_skip_cb cb_skip;                                    /* callback: skip some bytes from the  stream */
  void* cb_user;                                           /* pointer to data that is given to callback functions */
  int vid_in_w;                                            /* video input width */
  int vid_in_h;                                            /* video input height */
  int vid_out_w;                                           /* video output width */
  int vid_out_h;                                           /* video output height */
  int vid_fps;                                             /* video frame rate */
  vpx_img_fmt vid_fmt;                                     /* video input format, probably VPX_IMG_FMT_RGB24, if not set we use this one */
};

class WebmScreenRecorder {
 public:
  WebmScreenRecorder();
  ~WebmScreenRecorder();
  bool setup(WebmScreenRecorderSettings settings);
  bool start();
  bool stop();
  void grabFrame();
 private:
  EBML ebml;
  Webm webm;
  WebmScreenRecorderSettings settings;
  int nbytes_per_video_frame;                              /* ... */

#if !defined(WEBM_SCREENREC_USE_PBOS)
  unsigned char* pixels;                                   /* when not using pbos we use this to store the data */
#else
#endif
  
};

inline WebmScreenRecorderSettings::WebmScreenRecorderSettings()
                                  :cb_write(NULL),
                                  cb_close(NULL),
                                  cb_peek(NULL),
                                  cb_read(NULL),
                                  cb_skip(NULL),
                                  use_video(false),
                                  use_audio(false),
                                  vid_in_w(0),
                                  vid_in_h(0),
                                  vid_out_w(0),
                                  vid_out_h(0),
                                  vid_fmt(VPX_IMG_FMT_NONE)
{

}

inline WebmScreenRecorderSettings::~WebmScreenRecorderSettings() {
  cb_write = NULL;
  cb_close = NULL;
  cb_peek = NULL;
  cb_read = NULL;
  cb_skip = NULL;
  use_video = false;
  use_audio = false;
  vid_in_w = 0;
  vid_in_h = 0;
  vid_out_w = 0;
  vid_out_h = 0;
  vid_fmt = VPX_IMG_FMT_NONE;
}

#endif
