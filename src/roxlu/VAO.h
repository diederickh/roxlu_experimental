#ifndef VAOH
#define VAOH
// some nice info: http://www.opengl.org/wiki/Vertex_Array_Object
// vertex array object. (only mac for now)

#include "OpenGL.h"
#include "Error.h"

class VAO {
public:	
	VAO();
	VAO& bind();
	VAO& unbind();
private:
	GLuint vao_id;
};
#endif