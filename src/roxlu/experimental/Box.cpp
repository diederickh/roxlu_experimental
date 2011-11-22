#include "VertexData.h"
#include "Box.h"
namespace roxlu {

Box::Box() {
}

Box::~Box() {
}

void Box::create(float width, float height, float depth, VertexData& vd) {
	float x = width * 0.5;
	float y = height * 0.5;
	float z = depth * 0.5;

	// bottom
	// -------------------	
//	vd.addVertex(-x, -y, z);
//	vd.addVertex(x, -y, z);
//	vd.addVertex(x, -y, -z);
//	vd.addVertex(-x, -y, -z);
	vd.addVertex(-x, -y, -z);
	vd.addVertex(-x, -y, z);
	vd.addVertex(x, -y, z);
	vd.addVertex(x, -y, -z);
	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(0,-1,0);
	vd.addNormal(0,-1,0);
	vd.addNormal(0,-1,0);
	vd.addNormal(0,-1,0);
	vd.addQuad(0,1,2,3);
	
	// top
	// -------------------
//	vd.addVertex(-x, y, z);
//	vd.addVertex(x, y, z);
//	vd.addVertex(x, y, -z);
//	vd.addVertex(-x, y, -z);
	vd.addVertex(-x, y, -z);
	vd.addVertex(-x, y, z);
	vd.addVertex(x, y, z);
	vd.addVertex(x, y, -z);

	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(0,1,0);
	vd.addNormal(0,1,0);
	vd.addNormal(0,1,0);
	vd.addNormal(0,1,0);
	vd.addQuad(4,5,6,7);
	
	// back
	// -------------------
//	vd.addVertex(x,-y,-z);
//	vd.addVertex(-x,-y,-z);
//	vd.addVertex(-x,y,-z);
//	vd.addVertex(x,y,-z);
	vd.addVertex(-x,-y,-z);
	vd.addVertex(-x, y,-z);
	vd.addVertex(x,y,-z);
	vd.addVertex(x,-y,-z);

	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(0,0,-1);
	vd.addNormal(0,0,-1);
	vd.addNormal(0,0,-1);
	vd.addNormal(0,0,-1);
	vd.addQuad(8,9,10,11);
	
	// front
	// -------------------
//	vd.addVertex(-x, -y, z);
//	vd.addVertex(x, -y, z);
//	vd.addVertex(x, y, z);
//	vd.addVertex(-x, y, z);
	vd.addVertex(-x, -y, z);
	vd.addVertex(-x, y, z);
	vd.addVertex(x, y, z);
	vd.addVertex(x, -y, z);
	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addNormal(0,0,1);
	vd.addQuad(12,13,14,15);
	
	// left
	// -------------------
//	vd.addVertex(-x, -y, -z);
//	vd.addVertex(-x, -y, z);
//	vd.addVertex(-x, y, z);
//	vd.addVertex(-x, y, -z);
	vd.addVertex(-x, -y, -z);
	vd.addVertex(-x, -y, z);
	vd.addVertex(-x, y, z);
	vd.addVertex(-x, y, -z);
	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(-1,0,0);
	vd.addNormal(-1,0,0);
	vd.addNormal(-1,0,0);
	vd.addNormal(-1,0,0);
	vd.addQuad(16,17,18,19);
	
	// right
	// -------------------
//	vd.addVertex(x, -y, z);
//	vd.addVertex(x, -y, -z);
//	vd.addVertex(x, y, -z);
//	vd.addVertex(x, y, z);
	vd.addVertex(x, -y, -z);
	vd.addVertex(x, -y, z);
	vd.addVertex(x, y, z);
	vd.addVertex(x, y, -z);

	vd.addTexCoord(0,0);
	vd.addTexCoord(1,0);
	vd.addTexCoord(1,1);
	vd.addTexCoord(0,1);
	vd.addNormal(1,0,0);
	vd.addNormal(1,0,0);
	vd.addNormal(1,0,0);
	vd.addNormal(1,0,0);
	vd.addQuad(20,21,22,23);
}

}; // roxlu