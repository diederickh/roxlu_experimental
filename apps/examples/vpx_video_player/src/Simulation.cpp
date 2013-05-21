#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("IVF Player - press 1 to play, 2 to stop, 3 to pause");

  if(!player.open("video.ivf", true)) {
    RX_ERROR("Cannot load the video.ivf file from the data folder.");
    ::exit(EXIT_FAILURE);
  }

}

void Simulation::update() {
  player.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  player.draw(0,0);
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
    player.play();
  }
  else if(key == GLFW_KEY_2) {
    player.stop();
  }
  else if(key == GLFW_KEY_3) {
    player.pause();
  }
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
  ::exit(EXIT_SUCCESS);
}

