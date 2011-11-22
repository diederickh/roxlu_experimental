#ifndef ROXLU_R3FH
#define ROXLU_R3FH

#include <vector>
#include <string>
#include <map>

// Roxlu 3D Format: binary format to store vertex data // scenes which can 
// be imported into blender. See the blender directory of this roxlu lib.
// Use this addon in blender to import *.r3f files

using std::vector;
using std::string;
using std::map;

#define R3F_VERTEX_DATAS	1
#define R3F_SCENE_ITEMS		2
#define R3F_MATERIALS		3

namespace roxlu {

class IOBuffer;
class VertexData;
class SceneItem;
class Material;

class R3F {
public:
	enum Types {
		 VERTEX_DATAS
		,VERTICES
	};
	
	R3F();
	~R3F();
	inline void addVertexData(VertexData& vd);
	inline void addVertexData(VertexData* vd);
	inline void addSceneItem(SceneItem& si);
	inline void addSceneItem(SceneItem* si);
	inline void addMaterial(Material& m);
	inline void addMaterial(Material* m);
	
	
	void save(string fileName);
	void storeVertexData(IOBuffer& buffer, VertexData& vd); 
	void storeSceneItem(IOBuffer& buffer, SceneItem& vd); 
	void storeMaterial(IOBuffer& buffer, Material& m);
	void load(string fileName);
	
	inline VertexData* getVertexData(string name);
	SceneItem* getSceneItem(string name);
	Material* getMaterial(string name);
private:
	vector<Material*> materials;
	vector<VertexData*> vertex_datas;
	vector<SceneItem*> scene_items;
	
};

inline void R3F::addVertexData(VertexData& vd) {
	addVertexData(&vd);
}

inline void R3F::addVertexData(VertexData* vd) {
	vertex_datas.push_back(vd);
}

inline void R3F::addSceneItem(SceneItem& si) {
	addSceneItem(&si);
}

inline void R3F::addSceneItem(SceneItem* si){ 
	scene_items.push_back(si);
}

inline void R3F::addMaterial(Material& m) {
	addMaterial(&m);
}

inline void R3F::addMaterial(Material* m) {
	materials.push_back(m);
}



} // roxlu
#endif