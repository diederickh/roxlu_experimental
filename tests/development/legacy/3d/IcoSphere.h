#ifndef ROXLU_ICOSHPHEREH
#define ROXLU_ICOSHPHEREH
 
/*
#include "OpenGL.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Vertexdata.h"
*/
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/math/Vec2.h>
#include <roxlu/3d/VertexData.h>

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