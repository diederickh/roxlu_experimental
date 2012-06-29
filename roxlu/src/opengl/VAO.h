#ifndef ROXLU_VAOH
#define ROXLU_VAOH

// vao, when bound will store state of succesive calls to:
// - shader.enableVertexAttribArray()
// - vbo.setPosVertexAttribPointer
// - vbo.setNormVertexAttribPointer()
// - vbo.setTexVertexAttribPointer()
// some nice info: http://www.opengl.org/wiki/Vertex_Array_Object
// vertex array object. (only mac for now)

#include "Constants.h"
#include "OpenGL.h"
#include "Error.h"
namespace roxlu {

class VAO {
public:	
	VAO();
	VAO& bind();
	VAO& unbind();
private:
	GLuint vao_id;
};

} // roxlu
#endif