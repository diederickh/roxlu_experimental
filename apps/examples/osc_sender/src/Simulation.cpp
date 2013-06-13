#include <Simulation.h>

// ---------------------------------------------

Simulation::Simulation()
  :SimulationBase()
{

}

Simulation::~Simulation() {
  lo_address_free(osc_sender);
}

void Simulation::setup() {
  setWindowTitle("OSC sender example");

  // OSC SENDER SETUP
  osc_sender = lo_address_new(NULL, "1234"); // send on port 1234

  if(!osc_sender) {
    RX_ERROR("Cannot setup the OSC sender.");
    ::exit(EXIT_FAILURE);
  }
  
  // SETUP OPENGL
  gl::glr_init();
  
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();

  // DRAW THE LINE WE SENT TO THE RECEIVER
  gl::glr_color(4, 191, 157, 255); 
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
  lo_send(osc_sender, "/mouse", "ii", x, y); // Send two integers: "ii", to "/mouse" 
  points.push_back(Vec2(x,y));
}

void Simulation::onChar(int ch) {
}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

