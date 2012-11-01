#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
  ,gui("Settings", 300)
{
}

void Simulation::setup() {
  setWindowTitle("Example simulation");
  gui.addFloat("Value", some_value);
  gui.setColor(0.5f, 1.0f);
}

void Simulation::update() {
  gui.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT);
  gui.draw();
}

void Simulation::onMouseDown(int x, int y, int button) {
  gui.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {
  gui.onMouseUp(x,y);
}

void Simulation::onMouseDrag(int x, int y) {
}

void Simulation::onMouseMove(int x, int y) {
  gui.onMouseMoved(x,y);
}

void Simulation::onKeyDown(char key) {
}

void Simulation::onKeyUp(char key) {
}
