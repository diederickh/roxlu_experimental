#ifndef ROXLU_SPHEREH
#define ROXLU_SPHEREH

/*
#include "VertexData.h"
#include "Vec3.h"
#include "Vec2.h"
#include "OpenGL.h"
*/


/*

	WILL BE REPLACED BY UVSPHERE
	
*/
namespace roxlu {

class Sphere {
public:
	Sphere(float nRadius, float nStacks, float nSlices);
	~Sphere();
	
	// In createVertices() we add one stack (note the <= instead of <) because
	// we want quads to be used. Also we add one extra slice for the 
	// same reason; we could use less but then the texture mapping would 
	// be wrong (we would map the last slice item to the first one (0), 
	// which would have texcoord 0).
	void createVertices();
	void createIndices();
	
	
	void draw();
	int getAt(int nSliceItem, int nStackItem);
	void setVertexForIndex(int nIndex);
	VertexPTNC* getVertices();
	int getNumVertices();
	int* getIndices();
	int getNumIndices();
	VertexPTNC* vertices;
	int* index_elements;
	
	float radius;
	float stacks;
	float slices;
	int num_vertices;
	int num_index_elements;
};

} // roxlu

#endif