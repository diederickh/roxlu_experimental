#include <iostream>
#include <GL/glfw3.h>

//#if defined(ROXLU
//#include <GLXW/glxw.h>
//#include <GL/glfw3.h>

#include <roxlu/gl/OpenGLInit.h>

//#define GLEW_STATIC
//#include <GL/glew.h>

#include <string>

class SimulationBase {
public:
  SimulationBase();
  virtual ~SimulationBase();
  virtual void setup();
  virtual void update();
  virtual void draw();
  virtual void onMouseDown(int x, int y, int button);
  virtual void onMouseUp(int x, int y, int button);
  virtual void onMouseDrag(int x, int y, int dx, int dy, int button);
  virtual void onMouseMove(int x, int y);
  virtual void onChar(int ch); /* on keydown, gets character */
  virtual void onKeyDown(int key); /* get physical key so same for 'A' and 'a' */
  virtual void onKeyUp(int key); /* physical key */
  virtual void onWindowResize(int w, int h);
  virtual void onWindowClose();
  void setWindowTitle(const char* title);
  void showCursor();
  void hideCursor();
  void toggleCursor();
  std::string getClipboardString();
  void setClipboardString(std::string str);

public:
  unsigned int window_w;
  unsigned int window_h;
  int mouse_x;
  int mouse_y;
  int prev_mouse_x;
  int prev_mouse_y;
  int pressed_mouse_button;
  bool is_mouse_down;
  GLFWwindow* window;
};
