#include <glfw_wrapper/SimulationBase.h>

SimulationBase::SimulationBase()
  :is_mouse_down(false)
  ,mouse_x(0)
  ,mouse_y(0)
  ,prev_mouse_x(0)
  ,prev_mouse_y(0)
  ,pressed_mouse_button(0)
  ,window(NULL)
{
}

void SimulationBase::setup() {
}

void SimulationBase::update() {
}

void SimulationBase::draw() {
}

void SimulationBase::onMouseDown(int x, int y, int button) {
}

void SimulationBase::onMouseUp(int x, int y, int button) {
}

void SimulationBase::onMouseDrag(int x, int y, int dx, int dy, int button) {
}

void SimulationBase::onMouseMove(int x, int y) {
}

void SimulationBase::onKeyDown(char key) {
}

void SimulationBase::onKeyUp(char key) {
}

void SimulationBase::setWindowTitle(const char* title) {
  glfwSetWindowTitle(*window, title);
}

void SimulationBase::onWindowResize(int w, int h) {
  glViewport(0,0,w,h);
}

void SimulationBase::onWindowClose() {
}

