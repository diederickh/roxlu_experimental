#include <iostream>
#include <GL/glfw.h>

class SimulationBase {
public:
  void setWindowTitle(const char* tite);
};



// ----------------------
class Simulation : public SimulationBase {
public:
  void setup();
  void update();
  void draw();
  void onMouseDown(int x, int y);
  void onMouseUp(int x, int y);
  void onMouseDrag(int x, int y);
  void onMouseMove(int x, int y);
  void onKeyDown(char key);
  void onKeyUp(char key);
private:
};

void Simulation::setup() {
}


void Simulation::update() {
}

void Simulation::draw() {
}

void Simulation::onMouseDown(int x, int y) {
}

void Simulation::onMouseUp(int x, int y) {
}

void Simulation::onMouseDrag(int x, int y) {
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onKeyDown(char key) {
}

void Simulation::onKeyUp(char key) {
}

// ----------------------------------------------
int main() {
  int width = 1024;
  int height = 768;

  glfwInit();

  // set window properties
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // anti aliasing
  glfwOpenWindowHint(GLFW_REFRESH_RATE, 0); // 0 = system default

  // open the window.
  if(!glfwOpenWindow(width, height, 8, 8, 8, 8,32,8, GLFW_WINDOW)) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  Simulation sim;
  sim.setup();
  
  bool running = true;
  while(running) {

    sim.update();
    sim.draw();

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers();
    running = !(glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));
  }
  
  glfwTerminate();
  return 0;
};
