#include <glfw_wrapper/SimulationBase.h>
#include <buttons/Buttons.h>

class Simulation : public SimulationBase {
public:
  Simulation();
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y);
  void onMouseMove(int x, int y);
  void onKeyDown(char key);
  void onKeyUp(char key);
private:
  buttons::Buttons gui;
  float some_value;
  int g;
};
