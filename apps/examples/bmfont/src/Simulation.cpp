#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("BMFont example");

  glClearColor(0.1, 0.1, 0.1, 1.0);

  // Load the BMFont specs from `arial.fnt` (from the datapath)
  font.setup("arial.fnt", window_w, window_h, true);
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // we change the text every frame, so you must reset() it first
  font.reset();

  font.setColor(1.0f, 1.0f, 0.0f);
  font.addText("TIME: ", 10.0f, 10.0f);

  font.setColor(1.0f, 0.3f, 0.5f);
  font.addText(rx_strftime("%S"), 130.0f, 10.0f);

  font.update(); // <-- make sure that the vertices are sent to the gpu
  font.draw();
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

