#ifndef ROXLU_LEAP_LEAP_H
#define ROXLU_LEAP_LEAP_H

extern "C" {
#  include <uv.h>
}

#include <vector>
#include <leapmotion/Leap.h>

#define VERB_LEAP_INIT "on initialized"
#define VERB_LEAP_CONNECTION " on connection "
#define VERB_LEAP_EXIT "on exit"
#define VERB_LEAP_FOCUS_GAINED "on focus gained"
#define VERB_LEAP_FOCUS_LOST "on focus lost"

typedef void(*leap_motion_callback)(const Leap::Controller& controller, void* user);
typedef void(*leap_motion_frame_callback)(const Leap::Controller& controller, Leap::Frame& frame, void* user);

class LeapMotion : public Leap::Listener {
 public:
  LeapMotion();
  ~LeapMotion();

  void setup(leap_motion_callback cbConnect,
             leap_motion_frame_callback cbFrame,
             void* cbUser = NULL
  );
  void update();

  void onInit(const Leap::Controller& c);
  void onConnection(const Leap::Controller& c);
  void onExit(const Leap::Controller& c);
  void onFrame(const Leap::Controller& c);
  void onFocusGained(const Leap::Controller& c);
  void onFocusLost(const Leap::Controller& c);
 public:
  std::vector<Leap::Frame> frames;
  std::vector<Leap::Frame> work_frames;
  Leap::Controller controller;
  leap_motion_frame_callback cb_frame;
  leap_motion_callback cb_connect;
  void* cb_user;
  uv_mutex_t mutex;
};

#endif
