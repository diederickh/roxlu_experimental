#ifndef ROXLU_WEBCAM_H
#define ROXLU_WEBCAM_H

#include <videocapture/VideoCapture.h>
#include <videocapture/VideoCaptureGLSurface.h>
#include <av/AVEncoder.h> 
#include <av/AVEncoderThreaded.h> 
#include <audio/Audio.h>

void webcam_on_audio_in(const void* input, unsigned long nframes, void* user);
 
class Webcam {
 public:
  Webcam();
  ~Webcam();
  bool setup();
  void update();
  void draw();
 
  bool startCapture();
  bool stopCapture();
 
  bool startRecording();
  bool stopRecording();
 
  void lock();
  void unlock(); 
 
 public:
  bool must_capture;
  bool is_recording;
  VideoCapture cap;
  VideoCaptureGLSurface surf;
  Audio audio;
  AVEncoderThreaded enc;
};

#endif
