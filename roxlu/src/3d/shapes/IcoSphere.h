#ifndef ROXLU_ICOSHPHEREH
#define ROXLU_ICOSHPHEREH
 
#include "OpenGL.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Vertexdata.h"
#include <vector>

using namespace std;

namespace roxlu {

class IcoSphere {
public:
    IcoSphere();
    void create(int detail, float radius, VertexData& vertex_data);
private:
	
};

} // roxlu
#endif