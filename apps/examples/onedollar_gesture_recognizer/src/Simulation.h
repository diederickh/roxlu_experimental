#include <glfw_wrapper/SimulationBase.h>
#include <roxlu/Roxlu.h>
#include <osc/OSC.h>
#include <onedollar/OneDollar.h>
#include <glr/GL.h>

using namespace gl;

#define STATE_NONE 0
#define STATE_INPUT_GESTURE_KEY 1
#define STATE_INPUT_GESTURE_DRAW 2
#define STATE_INPUT_MATCH 3

class Simulation : public SimulationBase {
 public:
  Simulation();
  ~Simulation();
  void setup();
  void update();

  void draw();
  void drawStateNone();
  void drawStateGestureKey();
  void drawStateGestureDraw();
  void drawStateInputMatch();

  void onMouseDown(int x, int y, int buton);
  void onMouseUp(int x, int y, int button);
  void onMouseDrag(int x, int y, int dx, int dy, int button);
  void onMouseMove(int x, int y);
  void onChar(int ch); 
  void onKeyDown(int key);
  void onKeyUp(int key);
  void onWindowClose();
 public:
  int state;
  FPS fps;
  lo_address osc_client;
  OneDollar dollar;
  Gesture* gesture;
  Gesture* input_gesture;
  Gesture* matched_gesture;
  size_t gesture_index;

  char key_buf[512];
};
