#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <videocapture/VideoCaptureGLSurface.h>
#include <webm/VPXStream.h>

void on_frame_cb(unsigned char* pixels, size_t nbytes, void* user);

class Simulation : public SimulationBase {
 public:
  Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onChar(int ch); 
  void onKeyDown(int key);
  void onKeyUp(int key);
  void onWindowClose();
 public:
  VPXInStream in_stream;
  VideoCaptureGLSurface video_surface;
  FPS fps;
};
