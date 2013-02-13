#include <glfw_wrapper/SimulationBase.h>
#include <buttons/Buttons.h>
#include <buttons/Server.h>
#include <buttons/Panel.h>
#include <roxlu/Roxlu.h>

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
  void operator()(unsigned int dx);
 private:
  FPS fps;

  Server server;
  Panel gui;
  bool test_enabled;
  float test_slider;
  float test_color[4];
  int test_int;
  int test_vec[2];
  float test_vel[2];
  int test_recti[4];
  float test_rectf[4];
  float test_accelf[2];
  int test_option;
};
