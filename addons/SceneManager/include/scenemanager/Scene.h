#ifndef ROXLU_SCENE_H
#define ROXLU_SCENE_H

#include <time.h>

class SceneManager;

enum SceneStates {
  STATE_LEAVE   = 1
  ,STATE_ENTER
  ,STATE_ACTIVE
  ,STATE_SLEEP
};

class Scene {
 public:
  Scene(int name);
  int getName();
  int getState();
  void setSceneManager(SceneManager* sm);

  virtual void setState(int state) = 0;
  virtual bool setup() = 0;                             /* must be called by user */
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual bool onTimeout();                              /* when you've set timeout_after_seconds, this function will be called repeatedly until you change the 'timeout' value or go to another scene */
  virtual void updateTimeout();                          /* resets the timeout, call then e.g. when you want to extend the timeout time */
  SceneManager* scene_manager;

 public:
  int state;
  int name;
  unsigned int timeout_after_seconds;                    /* after X seconds we timeout this scene and call onTimeout() where you can do whatever you'd like */ 
  unsigned int timeout;                                  /* used to check if we need to timeout */
};

inline Scene::Scene(int name)
             :name(name)
            ,state(STATE_SLEEP)
            ,timeout(0)
            ,timeout_after_seconds(0)
{
}

inline int Scene::getState() {
  return state;
}	

inline int Scene::getName() {
  return name;
}

inline void Scene::setSceneManager(SceneManager* sm) {
  scene_manager = sm;
}

inline bool Scene::onTimeout() {
  return true;
}

inline void Scene::updateTimeout() {
  if(timeout_after_seconds) {
    time_t now = time(NULL);
    timeout = now + timeout_after_seconds;
  }
}
#endif
