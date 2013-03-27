#include <Simulation.h>

Simulation::Simulation()
  :SimulationBase()
{
}

void Simulation::setup() {
  setWindowTitle("FLOCKING");

  cam.setup(1024, 768);
  cam.setNear(0.01);
  cam.setFar(1000.0f);
  cam.translate(0.0, 0.0, 350.0);

  // CREATE PARTICLES
  float v = 0.02;
  float r = 250.0f;
  int num = 750;
  for(int i = 0; i < num; ++i) {
    RParticle p;
    p.pos.set(rx_random(-r, r),rx_random(-r,r), rx_random(-r,r));
    p.vel.set(rx_random(-v,v),rx_random(-v,v),rx_random(-v,v));
    p.vel_norm = p.vel;
    p.vel_norm.normalize();
    p.forces.set(0,0,1);
    ps.particles.push_back(p);
  }
  
  // CREATE PREDATORS
  int num_predators = 10;
  for(int i = 0; i < num_predators; ++i) {
    RParticle p;
    p.pos.set(rx_random(-r, r),rx_random(-r,r), rx_random(-r,r));
    p.vel.set(rx_random(-v,v),rx_random(-v,v),rx_random(-v,v));
    p.vel_norm = p.vel;
    p.vel_norm.normalize();
    ps.predators.push_back(p);
  }

  dt = (1.0/60.0) * 1000;  
  last_timestep = Timer::now();
  rdrawer.setup();

  FLVScreenRecorderSettings settings;
  settings.vid_in_w = window_w;
  settings.vid_in_h = window_h;
  settings.vid_out_w = settings.vid_in_w;
  settings.vid_out_h = settings.vid_in_h;
  settings.vid_fps = 30;
  if(!recorder.setup(settings)) {
    RX_ERROR(("ERROR: cannot setup recorder"));
    ::exit(EXIT_FAILURE);
  }
}

void Simulation::update() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  double now = Timer::now();
  double diff = now - last_timestep;

  // PREVENT "SPIRAL OF DEATH" (http://gafferongames.com/game-physics/fix-your-timestep/)
  if(diff >= 32) {
    diff = 32;
  }

  // TAKE AS MANY SMALL TIMESTEPS AS WE CAN
  time_accum += diff;
  while(time_accum >= dt) {
    ps.update(1.0f/60.0f);
    time_accum -= dt;
  }
  last_timestep = Timer::now();
}

void Simulation::draw() {
  const float* vm = cam.vm().getPtr();
  const float* pm = cam.pm().getPtr();
  const float* nm = cam.nm().getPtr();

  // DRAW PARTICLES/BOIDS
  rdrawer.mode = 0;
  rdrawer.draw(pm, vm, nm, ps.particles);

  // DRAW PREDATORS
  rdrawer.mode = 1;
  rdrawer.draw(pm, vm, nm, ps.predators);

  recorder.grabFrame();

  fps.draw();
}

void Simulation::onMouseDown(int x, int y, int button) {
  cam.onMouseDown(x,y);
}

void Simulation::onMouseUp(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  cam.onMouseDragged(x,y);
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onChar(int ch) {
  if(ch == 'r') {
    float r = 1.3;
    float v = 0.1;
    for(size_t i = 0; i < ps.particles.size(); ++i) {
      ps.particles[i].pos.set(rx_random(-r,r), rx_random(-r,r), rx_random(-r,r));
      ps.particles[i].vel.set(rx_random(-v,v), rx_random(-v,v), rx_random(-v,v));
      ps.particles[i].vel_norm = ps.particles[i].vel; 
      ps.particles[i].vel_norm.normalize();
    }
  }
  else if(ch == '1') {
    recorder.open(rx_to_data_path(rx_strftime("%Y_%m_%d_%H_%M_%S") +".flv"));
    recorder.start();
  }
  else if(ch == '2') {
    recorder.stop();
  }
}

void Simulation::onKeyDown(int key) {
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

