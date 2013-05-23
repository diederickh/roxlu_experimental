#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("Video recorder - Press 1 to start recording. Press 2 to stop the recording.");

  if(!cam.setup()) {
    RX_ERROR("Cannot setup the webcam and/or encoder; check if we're using the correct device and correct encoder settings.");
    ::exit(EXIT_FAILURE);
  }

  cam.startCapture();
}

void Simulation::update() {
  cam.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  cam.draw();
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
  if(key == GLFW_KEY_1) {
    cam.startRecording();
  }
  else if(key == GLFW_KEY_2) {
    cam.stopRecording();
  }
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
  cam.stopCapture();
  cam.stopRecording();
  ::exit(EXIT_SUCCESS);
}

