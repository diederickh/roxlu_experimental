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

using namespace std;

namespace roxlu {

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
	void exportSceneItem(SceneItem& si);
	void exportSceneItem(SceneItem* si);

private:
	
};


};

#endif