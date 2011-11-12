#include "Plane.h"
#include "VertexData.h"

namespace roxlu {

Plane::Plane() {
}

Plane::~Plane() {
}

void Plane::create(float width, float height, VertexData& vd) {
	float x = width * 0.5;
	float y = height * 0.5;
	float z = 0;

	vd.addVertex(-x, -y, z);
	vd.addVertex(x, -y, z);
	vd.addVertex(x, y, z);
	vd.addVertex(-x, y, z);
	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addQuad(0,1,2,3);	
}

};