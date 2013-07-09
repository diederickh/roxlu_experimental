#include <glfw_wrapper/SimulationBase.h>

// GLFW
// --------------------------------------------------------------
bool rx_glfw_init(int major, int minor) {
  glfwSetErrorCallback(rx_glfw_error_callback);

  if(!glfwInit()) {
    printf("ERORR: glfwInit() fails");
    return false;
  }

  // when using GL3, set some hints
#if defined(ROXLU_GL_CORE3)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  return true;
}

GLFWwindow* rx_glfw_create_window(int w, int h, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
  
  GLFWwindow* window = glfwCreateWindow(w, h, title, monitor, share);
  if(!window) {
    printf("ERROR: cannot open window.\n");
    return NULL;
  }

  // enable the window / gl context
  glfwMakeContextCurrent(window);

  // load GL extensions
  if(!rx_setup_opengl_extensions()) {
    printf("ERROR: Cannot setup opengl extensions.");
    return NULL;
  }

  return window;
}

bool rx_setup_opengl_extensions() {
#if defined(ROXLU_GL_CORE3) 
#  if defined(glxw_h)  // on mac glxw doesn't seem to make GL_RGB_422_APPLE available
  if(glxwInit() != 0) {
    return false;
  }
#  endif
#else
  glewExperimental = true;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    return false;
  }
#endif
  return true;
}

void rx_glfw_setup_simulation(SimulationBase* sim, GLFWwindow* window) {
  sim->window = window;

  glfwSetWindowUserPointer(window, sim);
  glfwGetWindowSize(window, &sim->window_w, &sim->window_h);
  glfwSetWindowSizeCallback(window, rx_glfw_window_size_callback);
  glfwSetWindowCloseCallback(window, rx_glfw_window_close_callback);
  glfwSetMouseButtonCallback(window, rx_glfw_mouse_button_callback);
  glfwSetCursorPosCallback(window, rx_glfw_cursor_callback);
  glfwSetScrollCallback(window, rx_glfw_scroll_callback);
  glfwSetKeyCallback(window, rx_glfw_key_callback);
  glfwSetCharCallback(window, rx_glfw_char_callback);

  sim->setup();
}

void rx_glfw_window_size_callback(GLFWwindow* window, int w, int h) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(sim_ptr) {
    sim_ptr->onWindowResize(w,h);
  }
}

void rx_glfw_window_close_callback(GLFWwindow* window) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(sim_ptr) {
    sim_ptr->onWindowClose();
  }
}

void rx_glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(!sim_ptr) {
    return;
  }
  if(action == GLFW_PRESS) {
    sim_ptr->onMouseDown(sim_ptr->mouse_x, sim_ptr->mouse_y, button, mods);
    sim_ptr->pressed_mouse_button = button;
    sim_ptr->is_mouse_down = true;
  }
  else {
    sim_ptr->onMouseUp(sim_ptr->mouse_x, sim_ptr->mouse_y, button, mods);
    sim_ptr->pressed_mouse_button = -1;
    sim_ptr->is_mouse_down = false;
  }
}

void rx_glfw_cursor_callback(GLFWwindow* window, double x, double y) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(!sim_ptr) {
    return;
  }
  sim_ptr->mouse_x = x;
  sim_ptr->mouse_y = y;

  if(sim_ptr->is_mouse_down) {
    sim_ptr->onMouseDrag(x, y, (x - sim_ptr->prev_mouse_x ), (sim_ptr->prev_mouse_y - y), sim_ptr->pressed_mouse_button);
  }
  else {
    sim_ptr->onMouseMove(x,y);
  }
  sim_ptr->prev_mouse_x = sim_ptr->mouse_x;
  sim_ptr->prev_mouse_y = sim_ptr->mouse_y;
}

void rx_glfw_scroll_callback(GLFWwindow* window, double x, double y) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(!sim_ptr) {
    return;
  }
}

void rx_glfw_char_callback(GLFWwindow* window, unsigned int ch) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(!sim_ptr) {
    return;
  }
  sim_ptr->onChar(ch);
}

void rx_glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  SimulationBase* sim_ptr = static_cast<SimulationBase*>(glfwGetWindowUserPointer(window));
  if(!sim_ptr) {
    return;
  }
  if(action == GLFW_PRESS) {
    sim_ptr->onKeyDown(key, scancode, mods);
  }
  else {
    sim_ptr->onKeyUp(key, scancode, mods);
  }
}

void rx_glfw_error_callback(int err, const char* msg) {
  printf("ERROR: %s, %d\n", msg, err);
}

// Simulation 
// --------------------------------------------------------------
SimulationBase::SimulationBase()
  :is_mouse_down(false)
  ,mouse_x(0)
  ,mouse_y(0)
  ,prev_mouse_x(0)
  ,prev_mouse_y(0)
  ,pressed_mouse_button(0)
  ,window(NULL)
  ,window_w(0)
  ,window_h(0)
{
}

SimulationBase::~SimulationBase() {
}

void SimulationBase::setup() {
}

void SimulationBase::update() {
}

void SimulationBase::draw() {
}

void SimulationBase::onMouseDown(double x, double y, int button, int mods) {
}

void SimulationBase::onMouseUp(double x, double y, int button, int mods) {
  
}

void SimulationBase::onMouseDrag(double x, double y, double dx, double dy, int button) {
}

void SimulationBase::onMouseMove(double x, double y) {
}

void SimulationBase::onChar(unsigned int ch) {
}

void SimulationBase::onKeyDown(int key, int scancode, int mods) {
}

void SimulationBase::onKeyUp(int key, int scancode, int mods) {
}

void SimulationBase::setWindowTitle(const char* title) {
  glfwSetWindowTitle(window, title);
}

void SimulationBase::onWindowResize(int w, int h) {
  glViewport(0,0,w,h);
}

void SimulationBase::onWindowClose() {
}

void SimulationBase::showCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void SimulationBase::hideCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void SimulationBase::toggleCursor() {

  int curr_cursor_mode = glfwGetInputMode(window, GLFW_CURSOR);
  if(curr_cursor_mode == GLFW_CURSOR_NORMAL) {
    hideCursor();
  }
  else {
    showCursor();
  }

}

void SimulationBase::setClipboardString(std::string str) {
  glfwSetClipboardString(window, str.c_str());
}

std::string SimulationBase::getClipboardString() {
  return glfwGetClipboardString(window);
}

void SimulationBase::closeWindow() {
  glfwSetWindowShouldClose(window, 1);
}
