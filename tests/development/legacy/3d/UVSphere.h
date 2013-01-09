#ifndef ROXLU_UVSPHEREH
#define ROXLU_UVSPHEREH

//#include "VertexData.h"

#include <roxlu/3d/VertexData.h>

namespace roxlu {

class UVSphere {
public:
	UVSphere();
	~UVSphere();
	void create(float radius, int phi, int theta, VertexData& vertex_data);
	int getAt(int nSliceItem, int nStackItem);
private:
	int slices;
	int stacks;
};

} // roxlu

#endif