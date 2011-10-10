#ifndef ROXLU_SCENEH
#define ROXLU_SCENEH

#include <vector>

using std::vector;

namespace roxlu {

class VertexData;
class VBO;
class SceneItem;

class Scene {
public:
	Scene();
	~Scene();
	
	inline void addVertexData(VertexData* vd);
	inline void addVBO(VBO* vbo);
	inline void addSceneItem(SceneItem* item);
		
	vector<VertexData*> vertex_datas;
	vector<VBO*> vbos;
	vector<SceneItem*> scene_items;
};


inline void Scene::addVertexData(VertexData* vd) {
	vertex_datas.push_back(vd);
}

inline void Scene::addVBO(VBO* vbo) {
	vbos.push_back(vbo);
}

inline void Scene::addSceneItem(SceneItem* item) {
	scene_items.push_back(item);
}


} // roxlu
#endif
