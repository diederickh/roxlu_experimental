#ifndef ROXLU_SCENEH
#define ROXLU_SCENEH

#include "SceneItem.h"
#include <vector>
#include <map>
#include <string>

using std::vector;
using std::map;
using std::string;

namespace roxlu {

class VertexData;
class VBO;
class Texture;
class Material;
class Light;

class Scene {
public:
	Scene();
	~Scene();
	typedef map<string, SceneItem*>::iterator	scene_item_iterator;
	typedef map<string, VBO*>::iterator			vbo_iterator;
	typedef map<string, VertexData*>::iterator	vertex_data_iterator;
	typedef map<string, Light*>::iterator		light_iterator;
	
	inline void addVertexData(string name, VertexData* vd);
	inline void addVertexData(string name, VertexData& vd);
	inline void addVBO(string name, VBO* vbo);
	inline void addVBO(string name, VBO& vbo);
	inline void addSceneItem(string name, SceneItem* item);
	inline void addSceneItem(string name, SceneItem& item);
	inline void addTexture(string name, Texture* tex);
	inline void addTexture(string name, Texture& tex);
	inline void addMaterial(string name, Material* mat);
	inline void addMaterial(string name, Material& mat);
	inline void addLight(string name, Light& light);
	inline void addLight(string name, Light* light);
	inline int getNumberOfLights();
	
	inline SceneItem* getSceneItem(string name);
	inline const map<string, SceneItem*>& getSceneItems() const;
	inline Material* getMaterial(string name);
	inline const map<string, Material*>& getMaterials() const;
	inline Light* getLight(string name);
	inline const map<string, VertexData*>& getVertexDatas() const;
	
	map<string, VertexData*> vertex_datas;
	map<string, VBO*> vbos;
	map<string, SceneItem*> scene_items;
	map<string, Texture*> textures;
	map<string, Material*> materials;
	map<string, Light*> lights;
};


inline SceneItem* Scene::getSceneItem(string name) {
	return scene_items[name];
}

inline void Scene::addVertexData(string name, VertexData& vd) {
	addVertexData(name, &vd);
}
inline void Scene::addVertexData(string name, VertexData* vd) {
	vertex_datas.insert(std::pair<string, VertexData*>(name, vd));
}

inline void Scene::addVBO(string name, VBO& vbo) {
	addVBO(name, &vbo);
}

inline void Scene::addVBO(string name, VBO* vbo) {
	vbos.insert(std::pair<string, VBO*>(name, vbo));
}


inline void Scene::addSceneItem(string name, SceneItem& item) {
	addSceneItem(name, &item);
}

inline void Scene::addSceneItem(string name, SceneItem* item) {
	item->setName(name);
	scene_items.insert(std::pair<string, SceneItem*>(name, item));
}

inline void Scene::addTexture(string name, Texture& tex) {
	addTexture(name, &tex);
}

inline void Scene::addTexture(string name, Texture* tex) {
	textures.insert(std::pair<string, Texture*>(name, tex));
}

inline void Scene::addMaterial(string name, Material& mat) {
	addMaterial(name, &mat);
}

inline void Scene::addMaterial(string name, Material* mat) {
	materials.insert(std::pair<string, Material*>(name, mat));
}

inline Material* Scene::getMaterial(string name) {
	map<string, Material*>::iterator it = materials.find(name);
	if(it != materials.end()) {
		return it->second;
	}
	return NULL;
}

inline void Scene::addLight(string name, Light& light) {
	addLight(name, light);
}

inline void Scene::addLight(string name, Light* light) {
	lights.insert(std::pair<string, Light*>(name, light));
}

inline Light* Scene::getLight(string name) {
	map<string, Light*>::iterator it = lights.find(name);
	if(it == lights.end()) {
		return NULL;
	}
	return it->second;
}

inline int Scene::getNumberOfLights() {
	return lights.size();
}

inline const map<string, Material*>& Scene::getMaterials() const {
	return materials;
}

inline const map<string, SceneItem*>& Scene::getSceneItems() const {
	return scene_items;
}

inline const map<string, VertexData*>& Scene::getVertexDatas() const {
	return vertex_datas;
}

} // roxlu
#endif
