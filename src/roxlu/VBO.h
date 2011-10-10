#ifndef VBOH
#define VBOH

#include "OpenGL.h"
#include "Error.h"

// I'm probably going to remove all functions like: setIndices, setTexCoords 
// and step over to using a couple of predefined Vertex structs so we can write
// special effects for them. For now (06.03.2011) we add these new types
// by adding thyeps to the VBOType struct.
#include "VertexTypes.h" 


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
	,VBO_TYPE_VERTEX_PTNC		= VERT_POS | VERT_TEX | VERT_NORM | VERT_COL
	,VBO_TYPE_VERTEX_PTNTB		= VERT_POS | VERT_TEX | VERT_NORM | VERT_TAN | VERT_BINORM
};

class VBO {
public:
	VBO();
	
	VBO& setVertexData(VertexData& rVertexData);
	VBO& setVertexData(VertexData* pVertexData);
	VBO& setVertexData(VertexP* pData, int nNum);
	VBO& setVertexData(VertexPN* pData, int nNum);
	VBO& setVertexData(VertexPT* pData, int nNum);
	VBO& setVertexData(VertexPNC* pData, int nNum);
	VBO& setVertexData(VertexPTN* pData, int nNum);
	VBO& setVertexData(VertexPTNC* pData, int nNum);
	VBO& setVertexData(VertexPTNTB* pData, int nNum);
	
	template<typename T>
	VBO& setVertexData(T* pData, int nNum, VBOType nType) {
		created_types = (hasIndices()) ? (VBO_TYPE_INDEX_ARRAY | nType) : nType;
		glGenBuffers(1,&vbo_vertices); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		glBufferData(GL_ARRAY_BUFFER, sizeof(T)*nNum, pData, GL_STREAM_DRAW); eglGetError();
		return *this;
	}

	VBO& updateVertexData(VertexData* pVertexData);

	VBO& setVertices(const float* pVertices, int nNumCoords, int nNum, int nUsage = GL_STATIC_DRAW);
	VBO& setTexCoords(const float* pTexCoords, int nNum, int nUsage = GL_STATIC_DRAW);
	VBO& setIndices(const int* pIndices, int nNum, int nUsage = GL_STATIC_DRAW);	
	VBO& setColors(const float* pColors, int nNumColors, int nUsage = GL_STATIC_DRAW);
	VBO& bind();
	VBO& unbind();
	VBO& drawArrays(int nDrawMode, int nFirst, int nTotal); // renamed form draw(),drawArrays and drawElements is more clear
	VBO& drawElements(int nDrawMode, int nTotal);
	
	bool hasType(VBOType nType) {
		return (nType & created_types);
	}
	
	bool hasIndices() {
		return hasType(VBO_TYPE_INDEX_ARRAY);
	}
	
	
	int created_types;
	
private:
	GLuint vbo_vertices; // this is used for the "combined" vertex struct VertexPT, VertexPTN etc..
	GLuint vbo_normals;  
	GLuint vbo_texcoords; 
	GLuint vbo_indices; 
	GLuint vbo_colors;
	
	int vertex_size;
	int vertex_stride;
};
#endif
