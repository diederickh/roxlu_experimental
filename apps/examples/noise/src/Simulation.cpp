#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("BMFont example");

  glClearColor(0.1, 0.1, 0.1, 1.0);
  printf("%f\n", Noise::noise(0.045f));
}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  float base_line = 300;
  float dy = 100;
  float height = sqrt(dy * dy);

  // NOISE
  // -------
  // The Noise::noise(), function expects a input in the range between 0.0f and 1.0f, in which is will 
  // calculate one period of noise. The returned values are in a range between -1.0f and 1.0f
  dd.begin(GL_LINES);
  dd.addVertex(0.0f,  base_line, 0.0f, 0.0f, 1.0f, 1.0f); 
  dd.addVertex(window_w, base_line, 0.0f, 0.0f, 1.0f, 1.0f); 
  dd.end();

  dd.begin(GL_LINE_STRIP);
  for(float i = 0; i < window_w; ++i) {
    float noise_val = Noise::noise(i * 0.01);
    float r = (noise_val < 0) ? 1.0f : 0.0;
    float g = (noise_val > 0) ? 1.0f : 0.0;
    dd.addVertex(i, base_line + noise_val * height, 0.0f, r, g, 0.0f); 
  }
  dd.end();


  // P-NOISE
  // -------
  // The Noise::pnoise(), function expects a input in the range between 0.0f and 1.0f and the number of 
  // periods to generate. The higher the number of periodes the 'denser' the noise will be.
  base_line = 500;
  dd.begin(GL_LINES);
  dd.addVertex(0.0f,  base_line, 0.0f, 0.0f, 1.0f, 1.0f); 
  dd.addVertex(window_w, base_line, 0.0f, 0.0f, 1.0f, 1.0f); 
  dd.end();

  float max_v = 0.0f;
  float min_v = 9999999999999.0f;

  dd.begin(GL_LINE_STRIP);
  for(float i = 0; i < window_w; ++i) {
    float noise_val = Noise::unoise(float(i) * 0.001 * mouse_x);

    if(noise_val > max_v) {
      max_v = noise_val;
    }

    if(noise_val < min_v) {
      min_v = noise_val;
    }

    float r = (noise_val < 0) ? 1.0f : 0.0;
    float g = (noise_val > 0) ? 1.0f : 0.0;
    dd.addVertex(i, base_line + noise_val * height, 0.0f, r, g, 0.0f); 
  }
  printf("min: %f, max: %f, delta: %f\n", min_v, max_v, (max_v - min_v));
  dd.end();

  dd.draw();
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

