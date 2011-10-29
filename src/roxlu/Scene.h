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
	typedef vector<SceneItem*>::iterator	scene_item_iterator;
	typedef vector<VBO*>::iterator 			vbo_iterator;
	typedef vector<VertexData*>::iterator	vertex_data_iterator;
	
	inline void addVertexData(VertexData* vd);
	inline void addVertexData(VertexData& vd);
	inline void addVBO(VBO* vbo);
	inline void addVBO(VBO& vbo);
	inline void addSceneItem(SceneItem* item);
	inline void addSceneItem(SceneItem& item);
		
	vector<VertexData*> vertex_datas;
	vector<VBO*> vbos;
	vector<SceneItem*> scene_items;
};


inline void Scene::addVertexData(VertexData& vd) {
	addVertexData(&vd);
}
inline void Scene::addVertexData(VertexData* vd) {
	vertex_datas.push_back(vd);
}

inline void Scene::addVBO(VBO& vbo) {
	addVBO(&vbo);
}

inline void Scene::addVBO(VBO* vbo) {
	vbos.push_back(vbo);
}


inline void Scene::addSceneItem(SceneItem& item) {
	addSceneItem(&item);
}

inline void Scene::addSceneItem(SceneItem* item) {
	scene_items.push_back(item);
}


} // roxlu
#endif
