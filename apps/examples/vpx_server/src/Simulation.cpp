#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
  ,out_stream(3344)
{

  VPXSettings s;
  s.in_w = 1024;
  s.in_h = 768;
  s.out_w = s.in_w;
  s.out_h = s.in_h;
  s.fps = 30;
  s.fmt = AV_PIX_FMT_BGRA; // <-- we grab in this fmt.
  out_stream.setup(s);

  if(!out_stream.start()) {
    RX_ERROR("Cannot start out server");
  }
  ax.setup(10);
  cam.setup(1024, 768);
}

void Simulation::setup() {
  setWindowTitle("Video server");
}

void Simulation::update() {
  out_stream.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();
  ax.draw(cam.pm().getPtr(), cam.vm().getPtr());
  out_stream.grabFrame();
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDown(int x, int y, int button) {
  cam.onMouseDown(x,y);
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  cam.onMouseDragged(x,y);
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

