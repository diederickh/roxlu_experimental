#ifndef ROXLU_EASYCAM_H
#define ROXLU_EASYCAM_H

#include <roxlu/3d/Camera.h>
#include <roxlu/3d/ArcBall.h>
#include <roxlu/math/Vec2.h>

namespace roxlu {

  class EasyCam : public Camera {
  public:
    EasyCam();
    void setup(float screenWidth, float screenHeight);
    void onMouseDown(float x, float y);
    void onMouseDragged(float x, float y, bool translate = false);
    void onScrollWheel(float d);
    ArcBall arcball;
  private:
    bool shift_down;
    Vec2 last_mouse_pos;
    float last_x;
    float last_y;
  };

} // roxlu
#endif
