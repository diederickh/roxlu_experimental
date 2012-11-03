#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
  ,gui("Settings", 300)
{
}

void Simulation::setup() {
  setWindowTitle("Example simulation");
  harvester.setup();
  cam.perspective(60.0f, 4.0f/3.0f, 0.1f, 10.0f);
  cam.setPosition(0.0f, 0.0f, -1.0f);
}

void Simulation::update() {
  //gui.update();
  harvester.update();
}

void Simulation::draw() {
  float n = Timer::millis() * 0.0001;
  glClearColor(0.5 + (0.5 * sin(n*10.4)), 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Simulation::onMouseDown(int x, int y, int button) {
  //  gui.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {
  //  gui.onMouseUp(x,y);
}

void Simulation::onMouseDrag(int x, int y) {
}

void Simulation::onMouseMove(int x, int y) {
  //  gui.onMouseMoved(x,y);
}

void Simulation::onKeyDown(char key) {
}

void Simulation::onKeyUp(char key) {
}
