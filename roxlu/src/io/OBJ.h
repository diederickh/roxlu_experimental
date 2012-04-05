#ifndef ROXLU_OBJH
#define ROXLU_OBJH
/*
#include "Vec2.h"
#include "Vec3.h"
#include "Scene.h"
#include "Quad.h"
#include "VertexData.h"
#include "Triangle.h"
*/
#include <sstream>
//#include "HalfEdge.h"
#include <vector>

using namespace std;

namespace roxlu {

class Material;
class SceneItem;

// OBJ 3D file exporter for roxlu lib. 
// - http://www.fileformat.info/format/material/
// - http://people.cs.kuleuven.be/~ares.lagae/libobj/index.html
// - http://people.sc.fsu.edu/~jburkardt/data/mtl/mtl.html
// - http://www.xmission.com/~nate/

class OBJ {
public:
	OBJ();
	~OBJ();
	inline void addSceneItem(SceneItem& si);
	inline void addSceneItem(SceneItem* si);
	inline void addSceneItems(vector<SceneItem*>& items);
	inline void addMaterial(Material* m);
	inline void addMaterial(Material& m);
	inline void addMaterials(vector<Material*>& mats);
	
	void exportSceneItem(SceneItem& si, stringstream& ss);
	void exportSceneItem(SceneItem* si, stringstream& ss);
	void createMaterialFile(Material& mat);
	bool save(string fileName, bool inDataPath = true);
private:
//	int64_t num_exported_vertices;
	vector<SceneItem*> scene_items;
	vector<Material*> materials;	
};

inline void OBJ::addSceneItem(SceneItem& si) {
	addSceneItem(&si);
}

inline void OBJ::addSceneItem(SceneItem* si) {
	scene_items.push_back(si);
}

inline void OBJ::addMaterial(Material& m) {
	addMaterial(&m);
}

inline void OBJ::addMaterial(Material* m) {
	materials.push_back(m);
}

inline void OBJ::addSceneItems(vector<SceneItem*>& items) {
	std::copy(items.begin(), items.end(), std::back_inserter(scene_items));
}

inline void OBJ::addMaterials(vector<Material*>& mats) {
	std::copy(mats.begin(), mats.end(), std::back_inserter(materials));
}

};

#endif