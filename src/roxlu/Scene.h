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

class Scene {
public:
	Scene();
	~Scene();
	typedef map<string, SceneItem*>::iterator	scene_item_iterator;
	typedef map<string, VBO*>::iterator			vbo_iterator;
	typedef map<string, VertexData*>::iterator	vertex_data_iterator;
	
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
	
	inline SceneItem* getSceneItem(string name);
	inline Material* getMaterial(string name);
	
	map<string, VertexData*> vertex_datas;
	map<string, VBO*> vbos;
	map<string, SceneItem*> scene_items;
	map<string, Texture*> textures;
	map<string, Material*> materials;
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

} // roxlu
#endif
