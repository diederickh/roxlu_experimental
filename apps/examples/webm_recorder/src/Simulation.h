#ifndef ROXLU_SIMULATION_H
#define ROXLU_SIMULATION_H

#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videocapture/VideoCapture.h>
#include <videocapture/VideoCaptureGLSurface.h>
#include <webm/WebmScreenRecorder.h>

extern "C" {
#  include <uv.h>
}

#define VIDEO_W 320
#define VIDEO_H 240
#define VIDEO_FPS 30

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

 public:
  VideoCapture cap;
  VideoCaptureGLSurface cap_gl;
  FPS fps;
  EBMLFile efile;
  WebmScreenRecorder rec;


};
#endif
