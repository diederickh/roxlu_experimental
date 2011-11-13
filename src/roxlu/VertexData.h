#ifndef ROXLU_VERTEXDATAH
#define ROXLU_VERTEXDATAH

#include "OpenGL.h"
#include "VertexTypes.h"
#include "Color.h"
#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Triangle.h"
#include "Quad.h"

#include <iostream>
#include <vector>

using std::endl;
using std::ostream;
using std::vector;

namespace roxlu {

class VertexData {
public:
	VertexData();
	int 			addVertex(const Vec3& rVec);
	int 			addVertex(const float nX, const float nY, const float nZ);
	void 			addTexCoord(const Vec2& rVec);
	void 			addTexCoord(const float nX, const float nY);
	void 			addColor(const Vec3& rColor);
	void 			addColor(const Color4& rColor);
	void 			addColor(const float nR, const float nG, const float nB);
	void			addNormal(const float nX, const float nY, const float nZ);
	void 			addNormal(const Vec3& rVec);
	void 			addIndex(const int& nIndex);
	int 			addTriangle(int a, int b, int c); 
	int				addTriangleAndIndices(int a, int b, int c);
	int 			addQuad(int nA, int nB, int nC, int nD);

	void debugDraw(int drawMode = GL_TRIANGLES);
	
	const float* 	getVerticesPtr();
	const float* 	getTexCoordsPtr();
	const float* 	getColorsPtr();
	const float* 	getNormalsPtr();
	const int* 		getIndicesPtr();

    Triangle* 		getTrianglePtr(int nTriangle);
	Quad* 			getQuadPtr(int nQuad);

	int 			getNumVertices();
	int 			getNumTexCoords();
	int 			getNumColors();
	int 			getNumNormals();
	int				getNumTangents();
	int 			getNumIndices();
	int 			getNumTriangles();
	int 			getNumQuads();
	Vec2			getTexCoord(int index);
	Vec2*			getTexCoordPtr(int index);
	Vec3 			getVertex(int nIndex);
	Vec3*			getVertexPtr(int nIndex);
	
	VertexP* 		getVertexP();  	
	VertexPT*		getVertexPT();
	VertexPN* 		getVertexPN();
	VertexPTN* 		getVertexPTN();
	VertexPTNT*		getVertexPTNT();
	VertexPNC* 		getVertexPNC();
	VertexPTNTB* 	getVertexPTNTB();
	
	inline void		clearAttribs();
	inline void		enablePositionAttrib();
	inline void		enableNormalAttrib();
	inline void		enableColorAttrib();
	inline void		enableTexCoordAttrib();
	inline void		disablePositionAttrib();
	inline void		disableNormalAttrib();
	inline void		disableColorAttrib();
	inline void		disableTexCoordAttrib();

	void			clear();
	
	inline void 	setVertex(int nDX, Vec3 oPosition) { vertices[nDX] = oPosition; }
	inline void 	setVertex(int nDX, float nX, float nY, float nZ) { vertices[nDX].set(nX, nY,nZ); }
	
	friend ostream& operator <<(ostream& os, const VertexData& data);
	
	void 			computeTangents();
	void			computeTangentForTriangle(Vec3& v1, Vec3& v2, Vec3& v3, Vec2& w1, Vec2& w2, Vec2& w3, Vec3& sdir, Vec3& tdir);
	
	void 			createTangentAndBiTangent(Vec3 va, Vec3 vb, Vec2 ta, Vec2 tb, Vec3& normal, Vec3& out_tangent, Vec3& out_bitangent);
	
	vector<Vec3>		normals;
	vector<Vec3>		vertices;
	vector<Vec2>		texcoords;
	vector<Color4> 	colors;
	vector<int> 		indices;
	vector<Triangle> 	triangles; 
	vector<Quad> 		quads;
	vector<Vec4>		tangents;
	vector<Vec3>		bitangents;
	
	int 				attribs;
	VertexP* 			vertex_p;
	VertexPTN* 			vertex_ptn;
	VertexPTNT*			vertex_ptnt;
	VertexPNC* 			vertex_pnc;
	VertexPN* 			vertex_pn;
	VertexPT*			vertex_pt;
	VertexPTNTB* 		vertex_ptntb;
};


// Set flags which should be used by a shader. When you add indices, texcoord,
// vertices etc.. by using addVertex, addIndex, etc.. the correct attributes
// are set automatically. But because the containers are public you can 
// work w/o these setters. For this purpose you can use these functions.
// -----------------------------------------------------------------------------
inline void VertexData::clearAttribs() {
	attribs = 0;
}

inline void	VertexData::enablePositionAttrib() {
	attribs |= VERT_POS;
}

inline void	VertexData::enableNormalAttrib() {
	attribs |= VERT_NORM;
}

inline void	VertexData::enableColorAttrib() {
	attribs |= VERT_COL;
}

inline void	VertexData::enableTexCoordAttrib() {
	attribs |= VERT_TEX;
}

inline void	VertexData::disablePositionAttrib() {
	attribs &= ~VERT_POS;
}

inline void	VertexData::disableNormalAttrib() {
	attribs &= ~VERT_NORM;
}

inline void	VertexData::disableColorAttrib() {
	attribs &= ~VERT_COL;
}

inline void	VertexData::disableTexCoordAttrib() {
	attribs &= ~VERT_TEX;
}

// -----------------------------------------------------------------------------
inline ostream& operator <<(ostream& os, const VertexData& data) {
	{
		os << "Vertices: (" << data.vertices.size() << ")" << endl;
		vector<Vec3>::const_iterator it = data.vertices.begin();
		while(it != data.vertices.end()) {
			os << (*it) << endl;
			++it;
		}
		os << endl;
	}
	{
		os << "Normals: (" << data.normals.size() << ")"  << endl;
		vector<Vec3>::const_iterator it = data.normals.begin();
		while(it != data.normals.end()) {
			os << (*it) << endl;
			++it;
		}
		os << endl;
	}
	{
		os << "TexCoords: (" << data.texcoords.size() << ")"  << endl;
		vector<Vec2>::const_iterator it = data.texcoords.begin();
		while(it != data.texcoords.end()) {
			os << (*it) << endl;
			++it;
		}
		os << endl;
	}
	return os;	
}


} // roxlu

#endif