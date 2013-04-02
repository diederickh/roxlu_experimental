/*
WebmScreenRecorder
------------------

This class is used to grab screen pixels and encode them into a Webm file.
You need to specify quite some settings before you can call the setup() function. 

This class makes use of the EBML buffer which stores data according to the 
EBML standard. The VPX encoder gives encoded data to the WebmScreenRecorder which
then muxes it into a .webm file using ebml. 

Sometime you want to write this ebml/webm bitstream to a socket and sometimes,
just to a file. To make this class reusable for different bitstreams you need to 
set a couple of EBML-callbacks in the 'WebmScreenRecorderSettings' variable you
pass into setup().

We created a basic helper to write webm to a file. This helper is called 'EBMLFile', and
some callback handlers:

     - ebml_file_write
     - ebml_file_close
     - ebml_file_peek
     - ebml_file_read
     - ebml_file_skip

So if you want to record to a file, you use:

       WebmScreenRecorder rec;
       EBMLFile efile;
       WebmScreenRecorderSettings c;
     
       c.use_video = true;
     
       c.cb_write = ebml_file_write;
       c.cb_close = ebml_file_close;
       c.cb_peek = ebml_file_peek;
       c.cb_read = ebml_file_read;
       c.cb_skip = ebml_file_skip;
       c.cb_user = &efile;
     
       c.vid_in_w = VIDEO_W;
       c.vid_in_h = VIDEO_H;
       c.vid_out_w = VIDEO_W;
       c.vid_out_h = VIDEO_H;
       c.vid_fps = VIDEO_FPS;
     
       if(!rec.setup(c)) {
         RX_ERROR("cannot setup screen recorder");
         ::exit(0);
       }
  
Then every frame/draw you call: rec.grabFrame(), the grabFrame() function
will make sure that we only grab a frame each time it's necessary (at the 
rate of 'vid_fps'.

We won't encode anything before you call 'start()' to start the encoding 
thread and 'stop()' to stop the encoding thread.

 */

#ifdef ROXLU_GL_WRAPPER

#ifndef ROXLU_WEBM_SCREENRECORDER_H
#define ROXLU_WEBM_SCREENRECORDER_H

//#define WEBM_SCREENREC_USE_PBOS 
#define WEBM_SCREENREC_NUM_PBOS

#include <string>
#include <roxlu/opengl/GL.h>
#include <webm/EBML.h>
#include <webm/Webm.h>

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
  AVPixelFormat vid_fmt;                                   /* video input format, probably VPX_IMG_FMT_RGB24, if not set we use this one */
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
                                  cb_user(NULL),
                                  use_video(false),
                                  use_audio(false),
                                  vid_in_w(0),
                                  vid_in_h(0),
                                  vid_out_w(0),
                                  vid_out_h(0),
                                  vid_fmt(AV_PIX_FMT_NONE)
{

}

inline WebmScreenRecorderSettings::~WebmScreenRecorderSettings() {
  cb_write = NULL;
  cb_close = NULL;
  cb_peek = NULL;
  cb_read = NULL;
  cb_skip = NULL;
  cb_user = NULL;
  use_video = false;
  use_audio = false;
  vid_in_w = 0;
  vid_in_h = 0;
  vid_out_w = 0;
  vid_out_h = 0;
  vid_fmt = AV_PIX_FMT_NONE;
}

#endif

#endif // ROXLU_GL_WRAPPER
