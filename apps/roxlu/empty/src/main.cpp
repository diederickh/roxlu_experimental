#include <Simulation.h>
#include <iostream>

Simulation* sim_ptr;

// CALLBACKS
// ---------
static void GLFWCALL window_size_callback(int w, int h);
int  GLFWCALL window_close_callback();
void GLFWCALL mouse_button_callback(int button, int action);
void GLFWCALL mouse_position_callback(int x, int y);
void GLFWCALL mouse_wheel_callback(int position);
void GLFWCALL key_callback(int key, int action);
void GLFWCALL char_callback(int chr, int action);


// APPLICATION ENTRY
// -----------------
int main() {
  int width = 1024;
  int height = 768;
  sim_ptr = NULL;


  // init
  glfwInit();

  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // anti aliasing
  glfwOpenWindowHint(GLFW_REFRESH_RATE, 0); // 0 = system default


  if(!glfwOpenWindow(width, height, 8, 8, 8, 8,32,8, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowSizeCallback(window_size_callback);
  glfwSetWindowCloseCallback(window_close_callback);
  glfwSetMouseButtonCallback(mouse_button_callback);
  glfwSetMousePosCallback(mouse_position_callback);
  glfwSetMouseWheelCallback(mouse_wheel_callback);
  glfwSetKeyCallback(key_callback);
  glfwSetCharCallback(char_callback);

  Simulation sim;
  sim_ptr = &sim;
  sim.setup();
  
  bool running = true;
  while(running) {
    glfwPollEvents();

    sim.update();
    sim.draw();

    glfwSwapBuffers();
    running = !(glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));
  }
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
};

// CALLBACK DEFS
// --------------
void GLFWCALL window_size_callback(int w, int h) {
  if(sim_ptr) {
    sim_ptr->onWindowResize(w,h);
  }
}

int GLFWCALL window_close_callback() {
  if(sim_ptr) {
    sim_ptr->onWindowClose();
  }
  return 1;
}

void GLFWCALL mouse_button_callback(int button, int action) {
  if(!sim_ptr) {
    return;
  }
   if(action == GLFW_PRESS) {
    sim_ptr->onMouseDown(sim_ptr->mouse_x, sim_ptr->mouse_y, button);
  }
   else {
    sim_ptr->onMouseUp(sim_ptr->mouse_x, sim_ptr->mouse_y, button);
  }
}

void GLFWCALL mouse_position_callback(int x, int y) {
  if(!sim_ptr) {
    return;
  }
  sim_ptr->prev_mouse_x = sim_ptr->mouse_x;
  sim_ptr->prev_mouse_y = sim_ptr->mouse_y;
  sim_ptr->mouse_x = x;
  sim_ptr->mouse_y = y;
  sim_ptr->onMouseMove(x,y);
}

void GLFWCALL mouse_wheel_callback(int position) {
  if(!sim_ptr) {
    return;
  }

}

void GLFWCALL key_callback(int key, int action) {
  if(!sim_ptr) {
    return;
  }
}

void GLFWCALL char_callback(int chr, int action) {
  if(!sim_ptr) {
    return;
  }
  if(action == GLFW_PRESS) {
    sim_ptr->onKeyDown(chr);
  }
  else {
    sim_ptr->onKeyUp(chr);
  }
}
