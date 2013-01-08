//#include "VBO.h"
//#include "VertexData.h"

#include <roxlu/opengl/VBO.h>
#include <roxlu/3d/VertexData.h>


/**
 * TODO: 06.03.2011, figure out if we shouldnt be using pre-defined
 * types of stucts with vertex data, so each vertex has all of it's 
 * attributes in one object. THOUGH THESE ARE USED BY THE MODELLOADER SO PROBABLY
 * WE NEED THESE TYPES AS WELL....
 *
 * This can be helpfull for creating predefined visual effects. 
 * See the book:  Game Engine Architecture, Jason Gregory, 2009. Page. 415.
 */ 

namespace roxlu {

VBO::VBO() 
:created_types(VBO_TYPE_NOT_USED)
{
	vbo_vertices = -1;
	vbo_texcoords = -1;
}

VBO& VBO::setVertexData(VertexData& rVertexData) {
	return setVertexData(&rVertexData);
}

VBO& VBO::setVertexData(VertexData* pData) {
	if(pData->getNumVertices() > 0) {
		setVertices(pData->getVerticesPtr(), 3, pData->getNumVertices(), GL_STATIC_DRAW);
	}
	
	if(pData->getNumTexCoords() > 0) {
		setTexCoords(pData->getTexCoordsPtr(), pData->getNumTexCoords(), GL_STATIC_DRAW);
	}
	
	if(pData->getNumIndices() > 0) {
		setIndices(pData->getIndicesPtr(), pData->getNumIndices(), GL_STATIC_DRAW);
	}
	
	if(pData->getNumColors() > 0) {
		setColors(pData->getColorsPtr(), pData->getNumColors(), GL_STATIC_DRAW);
	}
	return *this;
}

VBO& VBO::updateVertexData(VertexData* pVertexData) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * pVertexData->getNumVertices(), pVertexData->getVerticesPtr()); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0); eglGetError();
	return *this;
}

// we use the vbo_vertices for "combined" vertex data.
VBO& VBO::setVertexData(VertexP* pData, int nNum) {
	return setVertexData<VertexP>(pData, nNum, VBO_TYPE_VERTEX_P);
}
VBO& VBO::setVertexData(VertexPT* pData, int nNum) {
	return setVertexData<VertexPT>(pData, nNum, VBO_TYPE_VERTEX_PT);
}
VBO& VBO::setVertexData(VertexPN* pData, int nNum) {
	return setVertexData<VertexPN>(pData, nNum, VBO_TYPE_VERTEX_PN);
}
VBO& VBO::setVertexData(VertexPTN* pData, int nNum) {
	return setVertexData<VertexPTN>(pData, nNum, VBO_TYPE_VERTEX_PTN);
}
VBO& VBO::setVertexData(VertexPTNT* pData, int nNum) {
	return setVertexData<VertexPTNT>(pData, nNum, VBO_TYPE_VERTEX_PTNT);
}
VBO& VBO::setVertexData(VertexPNC* pData, int nNum) {
	return setVertexData<VertexPNC>(pData, nNum, VBO_TYPE_VERTEX_PNC);
}
VBO& VBO::setVertexData(VertexPTNC* pData, int nNum) {
	return setVertexData<VertexPTNC>(pData, nNum, VBO_TYPE_VERTEX_PTNC);
}
VBO& VBO::setVertexData(VertexPTNTB* pData, int nNum) {
	return setVertexData<VertexPTNTB>(pData, nNum, VBO_TYPE_VERTEX_PTNTB);
}


VBO& VBO::bind() {
// @todo maybe remove this class..
#if ROXLU_GL_MODE != ROXLU_GL_STRICT

	// VertexP
	if(created_types == VBO_TYPE_VERTEX_P) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		return *this;
	}
	// VertexPN
	else if(created_types == VBO_TYPE_VERTEX_PN) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		return *this; 
	}

	// VertexPT
	else if(created_types == VBO_TYPE_VERTEX_PT) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		return *this;
	}

	// vertices
	
	if(created_types & VBO_TYPE_VERTEX_ARRAY) {
		glEnableClientState(GL_VERTEX_ARRAY); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
		glVertexPointer(vertex_size, GL_FLOAT, vertex_stride, 0); eglGetError();
	} 

	
	// colors
	if(created_types & VBO_TYPE_COLOR_ARRAY) {
		glEnableClientState(GL_COLOR_ARRAY); eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_colors); eglGetError();
		glColorPointer(4, GL_FLOAT, sizeof(GLfloat) * 4, 0); eglGetError();
	}
	
	// texcoords
	if(created_types & VBO_TYPE_TEXCOORD_ARRAY) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);  eglGetError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords); eglGetError();
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vec2), 0); eglGetError();
	}
	
	// indices
	if(created_types & VBO_TYPE_INDEX_ARRAY) {	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices); eglGetError();
	}
#endif	
	return *this;
}

VBO& VBO::unbind() {
#if ROXLU_GL_MODE != ROXLU_GL_STRICT
	if(created_types & VBO_TYPE_VERTEX_ARRAY)	{ 
		glDisableClientState(GL_VERTEX_ARRAY);eglGetError();
	}
	if(created_types & VBO_TYPE_TEXCOORD_ARRAY)	{ 
		glDisableClientState(GL_TEXTURE_COORD_ARRAY); eglGetError();
	}	
	if(created_types & VBO_TYPE_COLOR_ARRAY)	{ 
		glDisableClientState(GL_COLOR_ARRAY); eglGetError();
	}	

#endif
	glBindBuffer(GL_ARRAY_BUFFER,0);
	return *this;
}


VBO& VBO::setVertices(const float* pVertices, int nNumCoords, int nNum, int nUsage) {
	// check input.	
	if(pVertices == NULL) {
		printf( "Incorrect vertex data");
		return *this;
	}
	
	vertex_size = nNumCoords;
	if(! (created_types & VBO_TYPE_VERTEX_ARRAY) ) {
		glGenBuffers(1, &vbo_vertices); eglGetError();
		created_types |= VBO_TYPE_VERTEX_ARRAY;
	}
	
	//printf("In VBO::setVertices, we need to test the sizeof check!, moved from ofVec3f to Vec3/Vec2\n");
	if(nNumCoords == 3) {
		vertex_stride = sizeof(Vec3);
	}
	else if (nNumCoords == 2) {
		vertex_stride = sizeof(Vec2);
	}
	else {
		printf("Incorrect number of coords given to setVertices. use 2 or 3, ofVec2|3f\n");
		return *this;
	}
	
	// Allocate and set the data.
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices); eglGetError();
	glBufferData(GL_ARRAY_BUFFER, nNum * vertex_stride, pVertices, nUsage); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0); eglGetError(); // do we ned this?
	return *this;
}

VBO& VBO::setColors(const float* pColors, int nNumColors, int nUsage) {
	// we assume 4f colors.
	if(pColors == NULL) {
		printf("Incorrect color data\n");
		return *this;
	}
	if(!(created_types & VBO_TYPE_COLOR_ARRAY)) {
		glGenBuffers(1, &vbo_colors); eglGetError();
		created_types |= VBO_TYPE_COLOR_ARRAY;
	} 
	glBindBuffer(GL_ARRAY_BUFFER, vbo_colors); eglGetError();
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * nNumColors, pColors, nUsage); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0); eglGetError();
	return *this; 
}

VBO& VBO::setIndices(const int* pIndices, int nNum, int nUsage) {
	if(pIndices == NULL) {
		printf("No correct indices given!\n");
		return *this;
	}
	
	if(!(created_types & VBO_TYPE_INDEX_ARRAY)) {
		glGenBuffers(1, &vbo_indices); eglGetError();
		created_types |= VBO_TYPE_INDEX_ARRAY;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_indices); eglGetError();
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * nNum, &pIndices[0], nUsage); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0); eglGetError();
	return *this;
}

VBO& VBO::setTexCoords(const float* pTexCoords, int nNum, int nUsage) {	
	if(pTexCoords == NULL) {
		printf("Incorrect texcoords array");
		return *this;
	}
	
	// create the VBO for texcoords.
	if(! (created_types & VBO_TYPE_TEXCOORD_ARRAY)) {
		glGenBuffers(1, &vbo_texcoords);eglGetError();
		created_types |= VBO_TYPE_TEXCOORD_ARRAY;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords); eglGetError();
	glBufferData(GL_ARRAY_BUFFER, nNum * sizeof(Vec2), pTexCoords, nUsage); eglGetError();
	glBindBuffer(GL_ARRAY_BUFFER, 0); eglGetError();
	return *this;
}

VBO& VBO::drawArrays(int nDrawMode, int nFirst, int nTotal) {
	if(created_types == VBO_TYPE_NOT_USED) {
		return *this;
	}
	bind();
		glDrawArrays(nDrawMode, nFirst, nTotal); eglGetError();
	unbind();
	return *this;
}

VBO& VBO::drawElements(int nDrawMode, int nTotal) {
	if(created_types == VBO_TYPE_NOT_USED) {
		return *this;
	}
	//cout << nTotal << endl;
	bind();	
        glDrawElements(nDrawMode, nTotal, GL_UNSIGNED_INT, NULL); eglGetError();
	unbind();
	return *this;
}

void VBO::setNormVertexAttribPointer(GLuint index) {
	// check if we have a normal
	if(!created_types & VERT_NORM) {
		printf("Cannot set norm attribute as we didnt foudn any.\n");
		return;
	}
	size_t stride = 0;
	int offset = -1;
	int type = created_types & ~(VBO_TYPE_INDEX_ARRAY);
	switch(type) {
		case VBO_TYPE_VERTEX_PN: {
			stride = sizeof(VertexPN);
			offset = offsetof(VertexPN, norm);
			break;
		}
		case VBO_TYPE_VERTEX_PTN: {
			stride = sizeof(VertexPTN);
			offset = offsetof(VertexPTN, norm);
			break;
		}
		case VBO_TYPE_VERTEX_PTNT: {
			stride = sizeof(VertexPTNT);
			offset = offsetof(VertexPTNT, norm);
			break;
		}
		case VBO_TYPE_VERTEX_PTNTB: {
			stride = sizeof(VertexPTNTB);
			offset = offsetof(VertexPTNTB, norm);
			break;
		}
		default: {
			offset = -1;
			break;
		}
	}
	
	printf("Set normal attrib at offset: %d\n", offset);
	// And set the attribute pointer.
	glVertexAttribPointer(index,3,GL_FLOAT,GL_FALSE,stride,(GLvoid*)offset);
}


void VBO::setPosVertexAttribPointer(GLuint index) {
	// check if we have a normal
	if(!created_types & VERT_POS) {
		printf("Cannot set pos attribute as we didnt found any.\n");
		return;
	}
	size_t stride = 0;
	int offset = -1;
	
	int type = created_types & ~(VBO_TYPE_INDEX_ARRAY);
	switch(type) {
		case VBO_TYPE_VERTEX_P: {
			stride = sizeof(VertexP);
			offset = 0;
			break;
		}
		case VBO_TYPE_VERTEX_PT: {
			stride = sizeof(VertexPT);
			offset = offsetof(VertexPT, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PN: {
			stride = sizeof(VertexPN);
			offset = offsetof(VertexPN, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PNC: {
			stride = sizeof(VertexPNC);
			offset = offsetof(VertexPNC, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PTN: {
			stride = sizeof(VertexPTN);
			offset = offsetof(VertexPTN, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PTNC: {
			stride = sizeof(VertexPTNC);
			offset = offsetof(VertexPTNC, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PTNT: {
			stride = sizeof(VertexPTNT);
			offset = offsetof(VertexPTNT, pos);
			break;
		}
		case VBO_TYPE_VERTEX_PTNTB: {
			stride = sizeof(VertexPTNTB);
			offset = offsetof(VertexPTNTB, norm);
			break;
		}
		default: {
			offset = -1;
			break;
		}
	}
	printf("Set position attrib at offset: %d\n", offset);
	// And set the attribute pointer.
	glVertexAttribPointer(index,3,GL_FLOAT,GL_FALSE,stride,(GLvoid*)offset);

}

void VBO::setTexVertexAttribPointer(GLuint index) {
	// check if we have a texcoord
	if(!created_types & VERT_TEX) {
	printf("Cannot set tex attribute as we didnt foudn any.\n");
		return;
	}
	size_t stride = 0;
	int offset = -1;
	int type = created_types & ~(VBO_TYPE_INDEX_ARRAY);
	switch(type) {
		case VBO_TYPE_VERTEX_PT: {
		
			stride = sizeof(VertexPT);
			offset = offsetof(VertexPT, tex);
			break;
		}
		case VBO_TYPE_VERTEX_PTN: {
			stride = sizeof(VertexPTN);
			offset = offsetof(VertexPTN, tex);
			break;
		}
		case VBO_TYPE_VERTEX_PTNC: {
			stride = sizeof(VertexPTNC);
			offset = offsetof(VertexPTNC, tex);
			break;
		}
		case VBO_TYPE_VERTEX_PTNT: {
			stride = sizeof(VertexPTNT);
			offset = offsetof(VertexPTNT, tex);
			break;
		}
		case VBO_TYPE_VERTEX_PTNTB: {
			stride = sizeof(VertexPTNTB);
			offset = offsetof(VertexPTNTB, tex);
			break;
		}
		default: {
			offset = -1;
			break;
		}
	}
	printf("Set tex attrib at offset: %d\n", offset);
	// And set the attribute pointer.
	glVertexAttribPointer(index,3,GL_FLOAT,GL_FALSE,stride,(GLvoid*)offset);

}



}