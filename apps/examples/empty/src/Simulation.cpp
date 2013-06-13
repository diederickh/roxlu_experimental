#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
{
}

void Simulation::setup() {
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Simulation::onMouseUp(double x, double y, int button, int mods) {
}

void Simulation::onMouseDown(double x, double y, int button, int mods) {
}

void Simulation::onMouseDrag(double x, double y, double dx, double dy, int button) {
}

void Simulation::onMouseMove(double x, double y) {
}

void Simulation::onChar(unsigned int ch) {
}

void Simulation::onKeyDown(int key, int scancode, int mods) {
  if(key == GLFW_KEY_ESCAPE) {
    closeWindow();
  }
}

void Simulation::onKeyUp(int key, int scancode, int mods) {
}

void Simulation::onWindowClose() {
  ::exit(EXIT_SUCCESS);
}

