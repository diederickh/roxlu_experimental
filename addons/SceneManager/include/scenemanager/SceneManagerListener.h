#ifndef ROXLU_SCENE_MANAGER_LISTENER_H
#define ROXLU_SCENE_MANAGER_LISTENER_H

class Scene;
class SceneManagerListener {
public:
  virtual void onSwitchScene(int nameFrome, int nameTo, Scene* sceneA, Scene* sceneB) = 0;
};
#endif
