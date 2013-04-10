#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
{

}

void Simulation::setup() {
  setWindowTitle("Simulation");
  tween.set(100.0, window_h-100, 1500, 0);
}

void Simulation::update() {
  tween.update();

  if(!tween.ready) {
    x += 6;
    line.push_back(Vec2(x, tween.value));
  }
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();

  // START - STOP VALUES
#if !defined(ROXLU_GL_CORE3)
  glLineWidth(1.0);
#endif

  dd.begin(GL_LINES);
  dd.addVertex(0, 100, 0.0, 0.8, 0.0, 0.2);
  dd.addVertex(window_w, 100, 0.0, 0.8, 0.0, 0.2);
  dd.addVertex(0, window_h-100, 0.0, 0.8, 0.0, 0.2);
  dd.addVertex(window_w, window_h-100, 0.0, 0.8, 0.0, 0.2);
  dd.end();

  // BALL 
  glPointSize(13.0f);
  dd.begin(GL_POINTS);
  dd.addVertex(window_w * 0.5, tween.getValue(), -0.2, 0.2, 0.8, 0.1);
  dd.end();
  dd.draw();

  // CURVE
  dd.begin(GL_LINE_STRIP);
  for(std::vector<Vec2>::iterator it = line.begin(); it != line.end(); ++it) {
    Vec2& v = *it;
    dd.addVertex(v.x, v.y, 0.0, 1.0, 1.0, 0.0, 1.0);
  }
  dd.end();
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
  line.clear();
  x = 0;

  if(key == '1') {
    tween.setType(TWEEN_LINEAR);
  }
  else if(key == '2') {
    tween.setType(TWEEN_IN_QUAD);
  }
  else if(key == '3') {
    tween.setType(TWEEN_OUT_QUAD);
  }
  else if(key == '4') {
    tween.setType(TWEEN_INOUT_QUAD);
  }
  else if(key == '5') {
    tween.setType(TWEEN_IN_CUBIC);
  }
  else if(key == '6') {
    tween.setType(TWEEN_OUT_CUBIC);
  }
  else if(key == '7') {
    tween.setType(TWEEN_INOUT_CUBIC);
  }
  else if(key == '8') {
    tween.setType(TWEEN_IN_QUART);
  }
  else if(key == '9') {
    tween.setType(TWEEN_OUT_QUART);
  }
  else if(key == '0') {
    tween.setType(TWEEN_INOUT_QUART);
  }
  else if(key == 'A') {
    tween.setType(TWEEN_IN_QUINT);
  }
  else if(key == 'B') {
    tween.setType(TWEEN_OUT_QUINT);
  }
  else if(key == 'C') {
    tween.setType(TWEEN_INOUT_QUINT);
  }
  else if(key == 'D') {
    tween.setType(TWEEN_IN_EXPO);
  }
  else if(key == 'E') {
    tween.setType(TWEEN_OUT_EXPO);
  }
  else if(key == 'F') {
    tween.setType(TWEEN_INOUT_EXPO);
  }
  else if(key == 'G') {
    tween.setType(TWEEN_IN_CIRC);
  }
  else if(key == 'H') {
    tween.setType(TWEEN_OUT_CIRC);
  }
  else if(key == 'I') {
    tween.setType(TWEEN_INOUT_CIRC);
  }
  else if(key == 'J') {
    tween.setType(TWEEN_IN_SINE);
  }
  else if(key == 'K') {
    tween.setType(TWEEN_OUT_SINE);
  }
  else if(key == 'L') {
    tween.setType(TWEEN_INOUT_SINE);
  }
  else if(key == 'M') {
    tween.setType(TWEEN_IN_BACK);
  }
  else if(key == 'N') {
    tween.setType(TWEEN_OUT_BACK);
  }
  else if(key == 'O') {
    tween.setType(TWEEN_INOUT_BACK);
  }
  else if(key == 'P') {
    tween.setType(TWEEN_IN_ELASTIC);
  }
  else if(key == 'Q') {
    tween.setType(TWEEN_OUT_ELASTIC);
  }
  else if(key == 'R') {
    tween.setType(TWEEN_INOUT_ELASTIC);
  }
  else if(key == 'S') {
    tween.setType(TWEEN_IN_BOUNCE);
  }
  else if(key == 'T') {
    tween.setType(TWEEN_OUT_BOUNCE);
  }

 tween.start();
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

