#include <scenemanager/SceneManagerListener.h>
#include <scenemanager/SceneManager.h>
#include <scenemanager/Scene.h>

SceneManager::SceneManager()
  :current_scene(NULL)
  ,current_dx(0)
  ,listener(NULL)
{
}

SceneManager::~SceneManager() {
}

void SceneManager::draw() {
  std::vector<Scene*>::iterator it = scenes.begin();
  while(it != scenes.end()) {
    (*it)->draw();
    ++it;
  }
}

void SceneManager::update() {
  std::vector<Scene*>::iterator it = scenes.begin();
  while(it != scenes.end()) {
    (*it)->update();
    ++it;
  }

  time_t now = time(NULL);
  if(current_scene 
     && current_scene->timeout
     && current_scene->timeout < now) 
    {
    current_scene->onTimeout();
  }
}

void SceneManager::switchScene(int sceneName) {
  Scene* from_scene_ptr = NULL;
  int from_scene_name = -1;
  if(current_scene != NULL) {
    from_scene_ptr = current_scene;
    from_scene_name =  current_scene->getName();
		
    if(from_scene_name == sceneName) {
      return;
    }

    current_scene->setState(STATE_LEAVE);
  }
  
  Scene* to_scene_ptr = NULL;
  int to_scene_name = -1;
  Scene* new_scene = findScene(sceneName);
  if(new_scene != NULL) {
    to_scene_ptr = new_scene;
    to_scene_name = sceneName;
    new_scene->setState(STATE_ENTER);
    current_scene = new_scene;
  }
	
  if(listener != NULL) {
    listener->onSwitchScene(from_scene_name, to_scene_name, from_scene_ptr, to_scene_ptr);
  }
 
  setSceneTimeout(current_scene);

  current_dx = findSceneIndex(current_scene);
}

void SceneManager::addScene(Scene* scene) {
  scenes.push_back(scene);
  scene->setSceneManager(this);
  scene->setState(STATE_SLEEP);
}

void SceneManager::nextScene() {
  if(!scenes.size()) {
    return;
  }
  ++current_dx %= scenes.size();
  if(current_scene != NULL) {
    current_scene->setState(STATE_LEAVE);
  }
  current_scene = scenes[current_dx];
  current_scene->setState(STATE_ENTER);
  setSceneTimeout(current_scene);
}

void SceneManager::previousScene() {
  if(current_dx == 0) {
    current_dx = scenes.size() -1;
  }
  else {
    current_dx--;
  }
  if(current_scene != NULL) {
    current_scene->setState(STATE_LEAVE);
  }

  current_scene = scenes[current_dx];
  current_scene->setState(STATE_ENTER);
  setSceneTimeout(current_scene);
}

void SceneManager::setSceneTimeout(Scene* s) {
  if(s->timeout_after_seconds != 0) {
    time_t now = time(NULL); 
    s->timeout = now + s->timeout_after_seconds;
  }
}

Scene* SceneManager::findScene(int name) {
  std::vector<Scene*>::iterator it = scenes.begin();
  while(it != scenes.end()) {
    if((*it)->getName() == name) {
      return (*it);
    }
    ++it;
  }
  return NULL;
}

void SceneManager::setListener(SceneManagerListener* listener) {
  this->listener = listener;
}
