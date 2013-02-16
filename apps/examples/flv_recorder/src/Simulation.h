#ifndef ROXLU_SIMULATION_H
#define ROXLU_SIMULATION_H

#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videocapture/VideoCapture.h>
#include <videocapture/VideoCaptureGLSurface.h>
#include <audio/Audio.h>
#include <flv/FLVScreenRecorder.h>


#define AUDIO_NUM_CHANNELS 1
#define AUDIO_SAMPLERATE 44100
#define AUDIO_MAX_SAMPLES 512 // max number of frames we receive in our audio callback
#define AUDIO_AV_FORMAT AV_FMT_INT16 // flv/av format
#define AUDIO_FORMAT paInt16  // PortAudio format
#define VIDEO_W 640
#define VIDEO_H 480
#define VIDEO_FPS 60

void audio_in_cb(const void* input, unsigned long nframes, void* user);

class Simulation : public SimulationBase {
public:
  Simulation();
  ~Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onKeyDown(int key);
  void onKeyUp(int key);
  void onWindowClose();
 private:
  void setupAudio();
  void setupCapture();
  void setupRecorder();
 public:
  FPS fps;

  /* Audio */
  Audio audio;


  /* VideoCapture + recording */
  VideoCapture cap;
  VideoCaptureGLSurface cap_surf;
  FLVScreenRecorder recorder;

};
#endif
