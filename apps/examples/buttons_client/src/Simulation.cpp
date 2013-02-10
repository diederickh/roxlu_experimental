#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
  ,client("localhost", "5050")
{
}

void Simulation::setup() {
  setWindowTitle("Buttons client");
  client.connect();
}

void Simulation::update() {
  client.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  client.draw();
  fps.draw();
}

void Simulation::onMouseDown(int x, int y, int button) {
  client.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {
  client.onMouseUp(x,y);
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  client.onMouseMoved(x,y);

}

void Simulation::onMouseMove(int x, int y) {
  client.onMouseMoved(x,y);
}

void Simulation::onChar(int ch) {
  if(ch == 's') {

  }
  else if(ch == 'l') {

  }
}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

