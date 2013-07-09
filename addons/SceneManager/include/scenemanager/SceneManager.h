#ifndef ROXLU_SCENE_MANAGER_H
#define ROXLU_SCENE_MANAGER_H

#include <vector>
#include <algorithm>
#include <time.h>

class SceneManagerListener;
class Scene;

class SceneManager {
 public:
  SceneManager();
  ~SceneManager();
  void setListener(SceneManagerListener* listener);
  void addScene(Scene* scene);
  void switchScene(int sceneName);
  Scene* findScene(int sceneName);
  int findSceneIndex(Scene* scene);
  void nextScene();
  void previousScene();
  void update();
  void draw();
  int getCurrentSceneName();
  void setSceneTimeout(Scene* scene);
 public:
  SceneManagerListener* listener;
  Scene* current_scene;
  int current_dx;
  std::vector<Scene*> scenes;	
};

inline int SceneManager::getCurrentSceneName() {
  return current_dx;
}

inline int SceneManager::findSceneIndex(Scene* scene) {
  return std::find(scenes.begin(), scenes.end(), scene) - scenes.begin();
}

#endif
