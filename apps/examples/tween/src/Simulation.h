#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <tween/Tween.h>
#include <vector>

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
  FPS fps;
  DebugDrawer dd;
  Tween<float> tween;
  std::vector<Vec2> line;
  float x;

};
