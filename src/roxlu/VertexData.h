#ifndef ROXLU_VERTEXDATAH
#define ROXLU_VERTEXDATAH

#include "VertexTypes.h"
#include "Color.h"
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
	void 			addColor(const Color4f& rColor);
	void 			addColor(const float nR, const float nG, const float nB);
	void			addNormal(const float nX, const float nY, const float nZ);
	void 			addNormal(const Vec3& rVec);
	void 			addIndex(const int& nIndex);
	int 			addTriangle(int nA, int nB, int nC); 
	int 			addQuad(int nA, int nB, int nC, int nD);

	const float* 	getVertices();
	const float* 	getTexCoords();
	const float* 	getColors();
	const float* 	getNormals();
	const int* 		getIndices();

    Triangle* 		getTriangle(int nTriangle);
	Quad* 			getQuad(int nQuad);

	int 			getNumVertices();
	int 			getNumTexCoords();
	int 			getNumColors();
	int 			getNumNormals();
	int 			getNumIndices();
	int 			getNumTriangles();
	int 			getNumQuads();
	Vec3 			getVertex(int nIndex);
	Vec3*			getVertexPtr(int nIndex);

	VertexP* 		getVertexP();  	
	VertexPN* 		getVertexPN();
	VertexPTN* 		getVertexPTN();
	VertexPNC* 		getVertexPNC();
	VertexPTNTB* 	getVertexPTNTB();
	
	void			clear();
	
	inline void 	setVertex(int nDX, Vec3 oPosition) { vertices[nDX] = oPosition; }
	inline void 	setVertex(int nDX, float nX, float nY, float nZ) { vertices[nDX].set(nX, nY,nZ); }
	
	friend ostream& operator <<(ostream& os, const VertexData& data);
	
	void 			calculateTangentAndBiTangent();
	void 			createTangentAndBiTangent(Vec3 va, Vec3 vb, Vec2 ta, Vec2 tb, Vec3& normal, Vec3& out_tangent, Vec3& out_bitangent);
	
	vector<Vec3>		normals;
	vector<Vec3>		vertices;
	vector<Vec2>		texcoords;
	vector<Color4f> 	colors;
	vector<int> 		indices;
	vector<Triangle> 	triangles;
	vector<Quad> 		quads;
	vector<Vec3>		tangents;
	vector<Vec3>		bitangents;
	
	int 				attribs;
	VertexP* 			vertex_p;
	VertexPTN* 			vertex_ptn;
	VertexPNC* 			vertex_pnc;
	VertexPN* 			vertex_pn;
	VertexPTNTB* 		vertex_ptntb;
};

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