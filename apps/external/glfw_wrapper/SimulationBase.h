#include <roxlu/opengl/GL.h>
#include <iostream>
#include <string>
#include <stdio.h>

class SimulationBase;

// GLFW
// --------------------------------------------------------------


bool rx_glfw_init(int major = 3, int minor = 2);
GLFWwindow* rx_glfw_create_window(int w, int h, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
void rx_glfw_setup_simulation(SimulationBase* sim, GLFWwindow* win);
void rx_glfw_init(SimulationBase* sim, GLFWwindow* window);
void rx_glfw_window_size_callback(GLFWwindow* window, int w, int h);
void rx_glfw_window_close_callback(GLFWwindow* window);
void rx_glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void rx_glfw_cursor_callback(GLFWwindow* window, double x, double y);
void rx_glfw_scroll_callback(GLFWwindow* window, double x, double y);
void rx_glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void rx_glfw_error_callback(int err, const char* msg);
void rx_glfw_char_callback(GLFWwindow* window, unsigned int chr);
bool rx_setup_opengl_extensions();


// Simulation 
// --------------------------------------------------------------
class SimulationBase {
public:
  SimulationBase();
  virtual ~SimulationBase();
  virtual void setup();
  virtual void update();
  virtual void draw();
  virtual void onMouseDown(double x, double y, int button, int mods);
  virtual void onMouseUp(double x, double y, int button, int mods);
  virtual void onMouseDrag(double x, double y, double dx, double dy, int button);
  virtual void onMouseMove(double x, double y);
  virtual void onChar(unsigned int ch );                                                   /* on keydown, gets character */
  virtual void onKeyDown(int key, int scancode, int mods);                                               /* get physical key so same for 'A' and 'a' */
  virtual void onKeyUp(int key, int scancode, int mods);                                                 /* physical key */
  virtual void onWindowResize(int w, int h);
  virtual void onWindowClose();
  void setWindowTitle(const char* title);
  void showCursor();
  void hideCursor();
  void toggleCursor();
  std::string getClipboardString();
  void setClipboardString(std::string str);
  void closeWindow();

public:
  int window_w;
  int window_h;
  double mouse_x;
  double mouse_y;
  double prev_mouse_x;
  double prev_mouse_y;
  
  int pressed_mouse_button;
  bool is_mouse_down;
  GLFWwindow* window;
};

