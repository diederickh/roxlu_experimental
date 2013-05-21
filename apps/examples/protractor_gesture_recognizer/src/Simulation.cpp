#include <Simulation.h>
#include <sstream>

#if defined(USE_LEAPMOTION)
void lm_connect(const Leap::Controller& controller, void* user) {
}

void lm_frame(const Leap::Controller& controller, Leap::Frame&  frame, void* user) {
  if(frame.fingers().count() == 1) {
    Simulation* s = static_cast<Simulation*>(user);
    const Leap::Vector p = frame.fingers()[0].tipPosition();
    float x = rx_map<float>(p.x, -150, 150, 0, 1024);
    float y = rx_map<float>(p.y, 5, 500, 768, 0);
    s->mouse_x = x;
    s->mouse_y = y;
    s->onMouseDrag(x, y, 0,0,0);
  }
}
#endif

// ----------------------------------------------------

Simulation::Simulation()
  :SimulationBase()
  ,gesture(NULL)
  ,state(STATE_NONE)
  ,realtime("realtime")
{

}

Simulation::~Simulation() {
  delete input_gesture;
  input_gesture = NULL;
}

void Simulation::setup() {
  setWindowTitle("Simulation");
  glr_init();

  protractor.load("gestures.txt", true);
  input_gesture = new Gesture("input");

#if defined(USE_LEAPMOTION)
  lm.setup(lm_connect, lm_frame, this);
#endif
}

void Simulation::update() {
#if defined(USE_LEAPMOTION)
  lm.update();
#endif
}

void Simulation::draw() {
  glClearColor(227/255.0f, 219/255.0f, 154/255.0f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  fps.draw();


  if(state == STATE_NONE) {
    drawNone();
  }
  else if(state == STATE_NEW_KEY) {
    drawNewKey();
  }
  else if(state == STATE_NEW_INPUT) {
    drawNewInput();
  }
  else if(state == STATE_MATCH) {
    drawMatch();
  }
  else if(state == STATE_MATCH_REALTIME) {
    drawRealtime();
  }
}

void Simulation::drawNone() {
  glr_draw_string("Press `N` to add a new gesture", 10, 10, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `M` to match with stored gestures", 10, 30, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `R` to match with continuously with gestures", 10, 50, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `S` to save the gestures", 10, 70, 0.0f, 0.0f, 0.0f);
}

void Simulation::drawNewKey() {
  glr_draw_string("Enter a character that we use as name for the gesture", 10, 10, 0.0f, 0.0f, 0.0f);
}

void Simulation::drawNewInput() {
  glr_draw_string("Press `SPACE` to add the gesture as template", 10, 10, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `X` to go back to start", 10, 30, 0.0f, 0.0f, 0.0f);

  glr_color(245, 61, 84);
  glr_begin(GL_LINE_STRIP);
  for(std::deque<Vec2>::iterator it = gesture->points.begin(); it != gesture->points.end(); ++it) {
    glr_vertex((*it));
  }
  glr_end();


  Vec2 c = gesture->centroid;

  glr_color(4, 191, 157);
  glr_begin(GL_LINE_STRIP);
  for(std::vector<Vec2>::iterator it = gesture->resampled_points.begin(); it != gesture->resampled_points.end(); ++it) {
    Vec2 p = c + (*it);
    glr_vertex(p);
  }
  glr_end();

  glPointSize(5.0f);
  glr_color(110, 242, 93); 
  glr_begin(GL_POINTS);

  for(std::vector<Vec2>::iterator it = gesture->resampled_points.begin(); it != gesture->resampled_points.end(); ++it) {
    Vec2 p = c + (*it);
    glr_vertex(p);
  }
  glr_end();
}

void Simulation::drawMatch() {
  glr_draw_string("Draw a gesture and that we will match with the stored gestures", 10, 10, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `X` to go back", 10, 30, 0.0f, 0.0f, 0.0f);

  glr_color(245, 61, 84);
  glr_begin(GL_LINE_STRIP);
  for(std::deque<Vec2>::iterator it = input_gesture->points.begin(); it != input_gesture->points.end(); ++it) {
    glr_vertex((*it));
  }
  glr_end();

#if defined(USE_LEAPMOTION)
  glPointSize(10.0f);
  glr_color(254, 131, 121);
  glr_begin(GL_POINTS);
  glr_vertex(Vec2(mouse_x, mouse_y));
  glr_end();
#endif
}

void Simulation::drawRealtime() {
  glr_draw_string("Draw a gesture and that we will match with the stored gestures", 10, 10, 0.0f, 0.0f, 0.0f);
  glr_draw_string("Press `X` to go back", 10, 30, 0.0f, 0.0f, 0.0f);

  glr_color(245, 61, 84);
  glr_begin(GL_LINE_STRIP);
  for(std::deque<Vec2>::iterator it = realtime.points.begin(); it != realtime.points.end(); ++it) {
    glr_vertex((*it));
  }
  glr_end();
}

void Simulation::onMouseUp(int x, int y, int button) {
  if(state == STATE_MATCH) {
    float score = 0.0f;
    matched = protractor.match(input_gesture, &score);
    RX_VERBOSE("SCORE: %f, MATCHED: %p", score, matched);
    if(matched) {
      RX_VERBOSE("NAME: %s", matched->name.c_str());
    }
    input_gesture->reset();
  }
}

void Simulation::onMouseDown(int x, int y, int button) {
}

void Simulation::onMouseDrag(int x, int y, int dx, int dy, int button) {
  if(state == STATE_NEW_INPUT) {
    gesture->addPoint(x,y);
  }
  else if(state == STATE_MATCH) {
    input_gesture->addPoint(x, y);
  }
  else if(state == STATE_MATCH_REALTIME) {
    realtime.addPoint(x,y);

    while(realtime.points.size() > 164) {
      realtime.points.erase(realtime.points.begin());
    }

    std::deque<Vec2> orig_points = realtime.points;
    std::deque<Vec2> points = realtime.points;
    int jump_size = 10;
    for(size_t i = 0; i < points.size(); ++i) {
      
      float score = 0.0f;
      realtime.points = points;
      Gesture* realtime_matched = protractor.match(&realtime, &score);

      if(realtime_matched) { 
        //RX_VERBOSE("SCORE: %f, MATCHED: %p", score, realtime_matched);
        //RX_VERBOSE("NAME: %s", realtime_matched->name.c_str());     
      }
      
      if(points.size() < jump_size) {
        break;
      }
      points.erase(points.begin(), points.begin() + jump_size);

    }

    realtime.points = orig_points;
  }
}

void Simulation::onMouseMove(int x, int y) {
}

void Simulation::onChar(int ch) {
}

void Simulation::onKeyDown(int key) {
  if(state == STATE_NONE) {
    if(key == GLFW_KEY_N) {
      state = STATE_NEW_KEY;
    }
    else if(key == GLFW_KEY_M) {
      state = STATE_MATCH;
      input_gesture->reset();
    }
    else if(key == GLFW_KEY_R) {
      state = STATE_MATCH_REALTIME;
    }
    else if(key == GLFW_KEY_S) {
      protractor.save("gestures.txt", true);
    }
  }
  else if(state == STATE_NEW_KEY) {
    std::stringstream ss;
    ss << (char)key;
    gesture = new Gesture(ss.str());
    state = STATE_NEW_INPUT;
  }
  else if(state == STATE_NEW_INPUT) {
    if(key == GLFW_KEY_SPACE) {
      protractor.addGesture(gesture);
      state = STATE_NONE;
    }
    else if(key == GLFW_KEY_X) {
       state = STATE_NONE;
       delete gesture;
    }
  }
  else if(state == STATE_MATCH) {
    if(key == GLFW_KEY_X) {
      state = STATE_NONE;
    }
  }
  else if(state == STATE_MATCH_REALTIME) {
    if(key == GLFW_KEY_X) {
      state = STATE_NONE;
    }
  }

}

void Simulation::onKeyUp(int key) {
}

void Simulation::onWindowClose() {
}

