#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("openGL");


  // YOU MUST CALL `glr_init` to initialize a global drawing object
  glr_init();

  if(!texture.load("shoe.png", true)) {
    RX_ERROR("Cannot load the logo");
  }
  texture.print();
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  fps.draw();

  glPointSize(14.0f);

  texture.draw(20,20, 320,240);

  glr_nofill();
  glr_draw_circle(200,350,100);

  glr_fill();
  glr_draw_circle(200,350,50);

  // SOME TEXT EXAMPLES
  glr_draw_string("Basic example of how to use the openGL addon", 10, 10);
  glr_draw_string("Sine..", 10, window_h * 0.5 + 70, 0.0, 1.0, 0.0);

  // SIN WAVE
  glr_begin(GL_LINE_STRIP);
  for(int i = 0; i < window_w; ++i) {
    float y = window_h * 0.5 + sin(i * TWO_PI * 0.001) * window_h * 0.2;
    if(i < (window_w * 0.5)) { 
      glr_color(Vec3(1.0, 0.0, 0.0));
    }
    else {
      glr_color(Vec3(0.0, 1.0, 0.0));
    }
    glr_vertex(Vec3(i, y));
  }
  glr_end();

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

