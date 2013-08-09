#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
{
}

void Simulation::setup() {
  setWindowTitle("BMFont example");

  glClearColor(0.1, 0.1, 0.1, 1.0);

  // Load the BMFont specs from `arial.fnt` (from the datapath)
  font.setup("arial.fnt", window_w, window_h, 1024, true);
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // IMPORTANT: we change the text every frame, so you must reset() it first
  font.reset();

  // -------------------------------------------

  // "TIME"
  font.setColor(1.0f, 1.0f, 0.0f);
  font.addText("TIME: ", 10.0f, 40.0f);

  // THE TIME VALUE
  font.setColor(1.0f, 0.3f, 0.5f);
  font.addText(rx_get_date_time_string(), 130.0f, 40.0f);

  // DRAW THE ADDED TEXT
  font.update(); // <-- make sure that the vertices are sent to the gpu
  font.draw();

  // -------------------------------------------


  // DRAW WITH CUSTOM MODEL MATRIX
  float angle = DEG_TO_RAD * (float(mouse_x) / window_w) * 90;
  angle = sin(rx_millis() * 0.001) * TWO_PI;
 
  font.setAlpha(1.0f);
  font.setColor(1.0f, 1.0f, 0.0f);
  size_t dx = font.addText("roxlu", 0.0, 0.0, 0.0, 0.0f);
  font.update();

  Mat4 model;
  model.rotateX(angle);
  model.setPosition(10, 100, -1.0);
  font.drawText(dx, model.getPtr());


  // Rotate with displaced vertices (last argument)
  dx = font.addText("roxlu", 0.0f, 0.0f, 0.0f, 10.0f);
  font.update();

  model.identity();
  model.rotateX(angle);
  model.setPosition(200, 90, -1.0f);
  font.drawText(dx, model.getPtr());

  //  fps.draw();
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

