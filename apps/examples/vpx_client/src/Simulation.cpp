#include <Simulation.h>

void on_frame_cb(unsigned char* pixels, size_t nbytes, void* user) {
  Simulation* p = static_cast<Simulation*>(user);
  p->video_surface.setPixels(pixels, nbytes);
}

Simulation::Simulation()
  :SimulationBase()
  ,in_stream("localhost","3344")
{

}

void Simulation::setup() {
  setWindowTitle("Video client");
  video_surface.setup(window_w, window_h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
  video_surface.flip(true, false);
  //  video_surface.rotate(90);
  in_stream.connect(on_frame_cb, this);
}

void Simulation::update() {
  in_stream.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  video_surface.draw(0,0,1024, 768);
  //  fps.draw();

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

