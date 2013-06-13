#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>

class Simulation : public SimulationBase {
 public:
  Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(double x, double y, int buton, int mods);
  void onMouseUp(double x, double y, int button, int mods);
  void onMouseDrag(double x, double y, double dx, double dy, int button);
  void onMouseMove(double x, double y);
  void onChar(unsigned int ch); 
  void onKeyDown(int key, int scancode, int mods);
  void onKeyUp(int key, int scancode, int mods);
  void onWindowClose();
 public:
};
