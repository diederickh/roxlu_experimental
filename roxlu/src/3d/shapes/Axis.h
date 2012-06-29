#ifndef ROXLU_AXISH
#define ROXLU_AXISH

#include "Vec3.h"
#include "VBO.h"
#include "VertexData.h"

namespace roxlu {

class Axis {
public:
	Axis();
	void setup(int nSize);
	void draw();
private:
	VBO vbo;
	VertexData vd;
	int num_vertices;
};

} // roxlu
#endif