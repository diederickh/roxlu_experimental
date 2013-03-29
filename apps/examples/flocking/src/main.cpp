#include <Simulation.h>
#include <iostream>

Simulation* sim_ptr;

// CALLBACKS
// ---------
void window_size_callback(GLFWwindow* window, int w, int h);
int window_close_callback(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action);
void cursor_callback(GLFWwindow* window, int x, int y);
void scroll_callback(GLFWwindow* window, double x, double y);
void key_callback(GLFWwindow* window, int key, int action);
void error_callback(int err, const char* msg);
void char_callback(GLFWwindow* window, int chr);


// APPLICATION ENTRY
// -----------------
int main() {
  int width = 1024;
  int height = 768;
  width = 640;
  height = 480;
  sim_ptr = NULL;

  int c = 0;
  GLFWmonitor** m = glfwGetMonitors(&c);

  // init
  glfwSetErrorCallback(error_callback);
  if(!glfwInit()) {
    printf("ERROR: cannot initialize GLFW.\n");
    exit(EXIT_FAILURE);
  }

  // glfwWindowHint(GLFW_DEPTH_BITS, 16);
  // glfwWindowHint(GLFW_FSAA_SAMPLES, 4);

  GLFWwindow* window = glfwCreateWindow(width, height, "Simulation", NULL, NULL);
  if(!window) {
    printf("ERROR: cannot open window.\n");
    exit(EXIT_FAILURE);
  }
  
  glfwSetWindowSizeCallback(window, window_size_callback);
  glfwSetWindowCloseCallback(window, window_close_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCharCallback(window, char_callback);

  glfwMakeContextCurrent(window);

#if defined(ROXLU_GL_CORE3)
  if(glxwInit() != 0) {
    printf("ERROR: cannot init glxw\n");
    return EXIT_FAILURE;
  }
#else
  glewExperimental = true;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    exit(EXIT_FAILURE);
  }
#endif

  Simulation sim;
  sim_ptr = &sim;
  sim.window = window;
  sim.window_w = width;
  sim.window_h = height;
  sim.setup();
  
  bool running = true;
  while(running) {
    glfwPollEvents();

    sim.update();
    sim.draw();

    glfwSwapBuffers(window);
    running = !(glfwGetKey(window, GLFW_KEY_ESC));
  }
  glfwTerminate();
  return EXIT_SUCCESS;
};

// CALLBACK DEFS
// --------------
void window_size_callback(GLFWwindow* window, int w, int h) {
  if(sim_ptr) {
    sim_ptr->onWindowResize(w,h);
  }
}

int window_close_callback(GLFWwindow* window) {
  if(sim_ptr) {
    sim_ptr->onWindowClose();
  }
  return GL_TRUE;
}

void mouse_button_callback(GLFWwindow* window, int button, int action) {
  if(!sim_ptr) {
    return;
  }
   if(action == GLFW_PRESS) {
    sim_ptr->onMouseDown(sim_ptr->mouse_x, sim_ptr->mouse_y, button);
    sim_ptr->pressed_mouse_button = button;
    sim_ptr->is_mouse_down = true;
  }
   else {
    sim_ptr->onMouseUp(sim_ptr->mouse_x, sim_ptr->mouse_y, button);
    sim_ptr->pressed_mouse_button = -1;
    sim_ptr->is_mouse_down = false;
  }
}

void cursor_callback(GLFWwindow* window, int x, int y) {
  if(!sim_ptr) {
    return;
  }
  sim_ptr->mouse_x = x;
  sim_ptr->mouse_y = y;

  if(sim_ptr->is_mouse_down) {
    sim_ptr->onMouseDrag(x, y, (sim_ptr->prev_mouse_x - x), (sim_ptr->prev_mouse_y - y), sim_ptr->pressed_mouse_button);
  }
  else {
    sim_ptr->onMouseMove(x,y);
  }
  sim_ptr->prev_mouse_x = sim_ptr->mouse_x;
  sim_ptr->prev_mouse_y = sim_ptr->mouse_y;
}

void scroll_callback(GLFWwindow* window, double x, double y) {
  if(!sim_ptr) {
    return;
  }
}

void char_callback(GLFWwindow* window, int ch) {
  if(!sim_ptr) {
    return;
  }
  sim_ptr->onChar(ch);
}

void key_callback(GLFWwindow* window, int key, int action) {
  if(!sim_ptr) {
    return;
  }
  if(action == GLFW_PRESS) {
    sim_ptr->onKeyDown(key);
  }
  else {
    sim_ptr->onKeyUp(key);
  }
}

void error_callback(int err, const char* msg) {
  printf("ERROR: %s, %d\n", msg, err);
}
