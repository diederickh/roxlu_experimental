#include <vector>
#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <osc/OSC.h>     /* include the OSC files */
#include <glr/GL.h>      /* include opengl debug drawer */

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
  osc::Receiver receiver;
  std::vector<Vec2> points;
};
