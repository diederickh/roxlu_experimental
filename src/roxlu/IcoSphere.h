#pragma once

#include "OpenGL.h"
#include "Vec3.h"
#include "Vec2.h"
#include <vector>

using namespace std;

class IcoSphere {
public:
    IcoSphere();
    void create(int detail, float radius);
	void draw();
	Vec3 pos;
private:
	vector<Vec3> verts;
	vector<Vec3> norms;
	vector<Vec2> coords;
	vector<int> indices;
};