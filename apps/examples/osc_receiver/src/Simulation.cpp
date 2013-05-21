#include <Simulation.h>


// These are the handlers for the OSC commands/paths.
// Each handler function must return 0 to indicate that
// the receiver should stop processing the input. All 
// other return values will result in the receiver 
// passing the current data to the next handler. 
// ---------------------------------------------

static int on_color(const char* path, const char* types, lo_arg** argv, int argc, void* data, void* user) {
  RX_VERBOSE("on_color: %f, %f, %f", argv[0]->f, argv[1]->f, argv[2]->f);

  return 0;
}

static int on_mouse(const char* path, const char* types, lo_arg** argv, int argc, void* data, void* user) {
  RX_VERBOSE("on_mouse: %d, %d", argv[0]->i, argv[1]->i);

  Simulation* sim = static_cast<Simulation*>(user);
  sim->points.push_back(Vec2(argv[0]->i, argv[1]->i));

  return 0;
}


// ---------------------------------------------

Simulation::Simulation()
  :SimulationBase()
  ,receiver("1234")
{

}

void Simulation::setup() {
  setWindowTitle("OSC receiver example");

  // SETUP OSC
  receiver.addMethod("/color", "fff", on_color, this); 
  receiver.addMethod("/mouse", "ii", on_mouse, this);
  receiver.start();

  // SETUP OPENGL
  gl::glr_init();
  
}

void Simulation::update() {
  receiver.update();
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();

  gl::glr_color(242, 232, 92);
  gl::glr_begin(GL_LINE_STRIP);
  for(std::vector<Vec2>::iterator it = points.begin(); it != points.end(); ++it) {
    gl::glr_vertex((*it));
  }
  gl::glr_end();
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

