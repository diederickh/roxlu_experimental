#ifndef ROXLU_VAOH
#define ROXLU_VAOH

// vao, when bound will store state of succesive calls to:
// - shader.enableVertexAttribArray()
// - vbo.setPosVertexAttribPointer
// - vbo.setNormVertexAttribPointer()
// - vbo.setTexVertexAttribPointer()
// some nice info: http://www.opengl.org/wiki/Vertex_Array_Object
// vertex array object. (only mac for now)

//#include "Roxlu.h"
/*
#include "Constants.h"
#include "OpenGL.h"
#include "Error.h"
*/

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/core/Constants.h>

namespace roxlu {

class VAO {
public:	
	VAO();
	void create();
	void bind();
	static void unbind();
	int getID();
private:
	GLuint vao_id;
};

inline int VAO::getID() {
	return vao_id;
}

} // roxlu
#endif
