#pragma once

#include "Vec3.h"
#include "VBO.h"
#include "VertexData.h"

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