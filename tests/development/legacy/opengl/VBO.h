#ifndef ROXLU_VBOH
#define ROXLU_VBOH

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/opengl/Error.h>
#include <roxlu/3d/VertexTypes.h>

//#include "OpenGL.h"
//#include "Error.h"

// I'm probably going to remove all functions like: setIndices, setTexCoords 
// and step over to using a couple of predefined Vertex structs so we can write
// special effects for them. For now (06.03.2011) we add these new types
// by adding thyeps to the VBOType struct.
//
// 
// Important, when you want us to create just one VBO, pass a VertexP*,PN*,etc.. 
// pointer.
//#include "VertexTypes.h" 

namespace roxlu {

class VertexData;

enum VBOType {
	 VBO_TYPE_NOT_USED			= 0
		 
	,VBO_TYPE_VERTEX_ARRAY		= ARRAY_VERT
	,VBO_TYPE_NORMAL_ARRAY		= ARRAY_NORM
	,VBO_TYPE_COLOR_ARRAY		= ARRAY_COL
	,VBO_TYPE_TEXCOORD_ARRAY	= ARRAY_TEX
	,VBO_TYPE_INDEX_ARRAY		= ARRAY_INDEX
	
	// new structs.
	,VBO_TYPE_VERTEX_P			= VERT_POS
	,VBO_TYPE_VERTEX_PN			= VERT_POS | VERT_NORM
	,VBO_TYPE_VERTEX_PT			= VERT_POS | VERT_TEX
	,VBO_TYPE_VERTEX_PNC		= VERT_POS | VERT_NORM | VERT_COL
	,VBO_TYPE_VERTEX_PTN		= VERT_POS | VERT_TEX | VERT_NORM
	,VBO_TYPE_VERTEX_PTNT		= VERT_POS | VERT_TEX | VERT_NORM | VERT_TAN
	,VBO_TYPE_VERTEX_PTNC		= VERT_POS | VERT_TEX | VERT_NORM | VERT_COL
	,VBO_TYPE_VERTEX_PTNTB		= VERT_POS | VERT_TEX | VERT_NORM | VERT_TAN | VERT_BINORM
};

class VBO {
public:
	VBO();
	
	VBO& setVertexData(VertexData& data);
	VBO& setVertexData(VertexData* data);
	VBO& setVertexData(VertexP* data, int num);
	VBO& setVertexData(VertexPN* data, int num);
	VBO& setVertexData(VertexPT* data, int num);
	VBO& setVertexData(VertexPNC* data, int num);
	VBO& setVertexData(VertexPTN* data, int num);
	VBO& setVertexData(VertexPTNT* data, int num);
	VBO& setVertexData(VertexPTNC* data, int num);
	VBO& setVertexData(VertexPTNTB* data, int num);
	
	
	template<typename T>
	VBO& setVertexData(T* data, int num, VBOType nType) {
		created_types = (hasIndices()) ? (VBO_TYPE_INDEX_ARRAY | nType) : nType;
		glGenBuffers(1,&vbo_vertices); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(T)*num, data, GL_STREAM_DRAW); eglGetError();
		return *this;
	}

	VBO& updateVertexData(VertexData* data);

	VBO& setVertices(const float* vertices, int numCoords, int num, int usage = GL_STATIC_DRAW);
	VBO& setTexCoords(const float* texCoords, int num, int usage = GL_STATIC_DRAW);
	VBO& setIndices(const int* indices, int num, int usage = GL_STATIC_DRAW);	
	VBO& setColors(const float* colors, int numColors, int usage = GL_STATIC_DRAW);
	VBO& bind();
	VBO& unbind();
	VBO& drawArrays(int drawMode, int first, int total); // renamed form draw(),drawArrays and drawElements is more clear
	VBO& drawElements(int drawMode, int total);
	
	// set vertex-attrbute pointers (glVertexAttribPointer);
	void setNormVertexAttribPointer(GLuint index);
	void setPosVertexAttribPointer(GLuint index);
	void setTexVertexAttribPointer(GLuint index);
	
	
	bool hasType(VBOType type) {
		return type;
	}
	
	bool hasIndices() {
		return hasType(VBO_TYPE_INDEX_ARRAY);
	}
	
	VBO& operator=(VertexData& vertexData); // copy vertices into vbo
	
	int created_types;
	
private:
	void createIndexBuffer();
	GLuint vbo_vertices; // this is used for the "combined" vertex struct VertexPT, VertexPTN etc..
	GLuint vbo_normals;  
	GLuint vbo_texcoords; 
	GLuint vbo_indices; 
	GLuint vbo_colors;
	
	int vertex_size;
	int vertex_stride;
};

inline VBO& VBO::operator=(VertexData& vertexData) {
	return setVertexData(vertexData);
}

} // roxlu
#endif
