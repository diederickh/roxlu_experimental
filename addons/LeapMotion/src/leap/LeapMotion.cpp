#include <leap/LeapMotion.h>
#include <roxlu/core/Log.h>
#include <algorithm>

LeapMotion::LeapMotion() 
  :cb_frame(NULL)
  ,cb_connect(NULL)
  ,cb_user(NULL)
{
  uv_mutex_init(&mutex);
}

LeapMotion::~LeapMotion() {

  controller.removeListener(*this);

  cb_frame = NULL;
  cb_connect = NULL;
  cb_user = NULL;

  uv_mutex_destroy(&mutex);
}

void LeapMotion::setup(leap_motion_callback cbConnection,
                       leap_motion_frame_callback cbFrame, 
                       void* cbUser) 
{
  cb_frame = cbFrame;
  cb_connect = cbConnection;
  cb_user = cbUser;
  controller.addListener(*this);
}

void LeapMotion::update() {
  
  uv_mutex_lock(&mutex);
  if(cb_frame && frames.size()) {
    std::copy(frames.begin(), frames.end(), std::back_inserter(work_frames));
    frames.clear();
  }
  uv_mutex_unlock(&mutex);

  if(cb_frame && work_frames.size()) {
    for(std::vector<Leap::Frame>::iterator it = work_frames.begin(); it != work_frames.end(); ++it) {
      Leap::Frame fr = *it;
      cb_frame(controller, *it, cb_user);
    }
    work_frames.clear();
  }
}

void LeapMotion::onInit(const Leap::Controller& c) {
  RX_VERBOSE(VERB_LEAP_INIT);

}

void LeapMotion::onConnection(const Leap::Controller& c) {
  RX_VERBOSE(VERB_LEAP_CONNECTION);
  if(cb_connect) {
    cb_connect(*this, cb_user);
  }
}

void LeapMotion::onExit(const Leap::Controller& c) {
  RX_VERBOSE(VERB_LEAP_EXIT);
}

void LeapMotion::onFrame(const Leap::Controller& c) {

  if(cb_frame) {
    uv_mutex_lock(&mutex);
    frames.push_back(c.frame());
    uv_mutex_unlock(&mutex);
  }
}

void LeapMotion::onFocusGained(const Leap::Controller& c) {
  RX_VERBOSE(VERB_LEAP_FOCUS_GAINED);
}

void LeapMotion::onFocusLost(const Leap::Controller& c) {
  RX_VERBOSE(VERB_LEAP_FOCUS_LOST);
}

