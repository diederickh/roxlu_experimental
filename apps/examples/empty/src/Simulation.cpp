#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("Simulation");
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onChar(int ch) {
}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

