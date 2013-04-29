#include <Simulation.h>


Simulation::Simulation()
  :SimulationBase()
  ,gesture_index(0)
  ,state(STATE_NONE)
  ,gesture(NULL)
{
  input_gesture = new Gesture("input");
}

Simulation::~Simulation() {
  lo_address_free(osc_client);
}

void Simulation::setup() {
  setWindowTitle("Simulation");
  osc_client = lo_address_new(NULL, "1234");

  glr_init();

  dollar.load("gestures.txt", true);
}

void Simulation::update() {
}

void Simulation::draw() {
  glClearColor(227/255.0f, 219/255.0f, 154/255.0f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(state == STATE_NONE) {
    drawStateNone();
  }
  else if(state == STATE_INPUT_GESTURE_KEY) {
    drawStateGestureKey();
  }
  else if(state == STATE_INPUT_GESTURE_DRAW) {
    drawStateGestureDraw();
  }
  else if(state == STATE_INPUT_MATCH) {
    drawStateInputMatch();
  }
}

void Simulation::drawStateNone() {
  glr_draw_string("Press `N` to add a new gesture", 400, 340, 0,0,0);
  glr_draw_string("Press `S` to save all added gestures", 400, 360, 0,0,0);
  glr_draw_string("Press `M` to match gestures", 400, 380, 0,0,0);
  
}

void Simulation::drawStateGestureKey() {
  glr_draw_string("Press a key to name the gesture (e.g. A, B, C ..)", 330, 340, 0,0,0);
}

void Simulation::drawStateGestureDraw() {

  glr_draw_string("Draw a gesture and press `SPACE` when ready or `X` to start clear.", 300, 40, 0,0,0);

  glLineWidth(2.0f);
  glr_color(245, 61, 84);
  glr_begin(GL_LINE_STRIP);
  for(std::vector<Vec2>::iterator it = gesture->points.begin(); it != gesture->points.end(); ++it) {
    glr_vertex((*it));
  }
  glr_end();

}

void Simulation::drawStateInputMatch() {
  glr_draw_string("Draw a gesture or `X` to go back to menu.", 350, 40, 0,0,0);

  glLineWidth(2.0f);
  glr_color(119, 134, 145); 
  glr_begin(GL_LINE_STRIP);
  for(std::vector<Vec2>::iterator it = input_gesture->points.begin(); it != input_gesture->points.end(); ++it) {
    glr_vertex((*it));
  }
  glr_end();

  if(matched_gesture) {
    glr_color(64, 194, 91); 
    glr_begin(GL_LINE_STRIP);
    for(std::vector<Vec2>::iterator it = matched_gesture->resampled_points.begin(); it != matched_gesture->resampled_points.end(); ++it) {
      Vec2 v = *it;
      v.x += 250;
      v.y += 250;
      glr_vertex(v);
    }
    glr_end();
    
  }
}

void Simulation::onMouseUp(int x, int y, int button) {
  if(state == STATE_INPUT_MATCH) {
    double score = 0.0f;
    matched_gesture = dollar.match(input_gesture, &score);
    input_gesture->reset();
  }
}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  if(state == STATE_INPUT_GESTURE_DRAW && gesture) {
    gesture->addPoint(x,y);
  }
  else if(state == STATE_INPUT_MATCH) {
    input_gesture->addPoint(x,y);
  }
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onChar(int ch) {
}

void Simulation::onKeyDown(int key) {
  if(state == STATE_NONE) {
    if(key == GLFW_KEY_N) {
      state = STATE_INPUT_GESTURE_KEY;
    }
    else if(key == GLFW_KEY_S) {
      dollar.save("gestures.txt", true);
    }
    else if(key == GLFW_KEY_M) {
      input_gesture->reset();
      state = STATE_INPUT_MATCH;
    }
  }
  else if(state == STATE_INPUT_GESTURE_KEY) {
    if(key == GLFW_KEY_SPACE) {
      return;
    }

    state = STATE_INPUT_GESTURE_DRAW;
    sprintf(key_buf,"%c", (unsigned char)key);  
    key_buf[1] = '\0';
    gesture = new Gesture(key_buf);
  }
  else if(state == STATE_INPUT_GESTURE_DRAW) {
    if(key == GLFW_KEY_X) {
      gesture->reset();
    }
    else if(key == GLFW_KEY_SPACE) {
      dollar.addGesture(gesture);
      state = STATE_NONE;
    }
  }
  else if(state == STATE_INPUT_MATCH) {
    if(key == GLFW_KEY_X) {
      state = STATE_NONE;
    }
  }
}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

