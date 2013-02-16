#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <webm/VPXStream.h>

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
 private:
  VPXOutStream out_stream;
  FPS fps;
  EasyCam cam;
  Axis ax;
};
