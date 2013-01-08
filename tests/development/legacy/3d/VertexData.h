#ifndef ROXLU_VERTEXDATAH
#define ROXLU_VERTEXDATAH

/*
#include "OpenGL.h"
#include "VertexTypes.h"
#include "Color.h"
#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Triangle.h"
#include "Quad.h"
*/

#include <roxlu/graphics/Color.h>
#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/3d/VertexTypes.h>
#include <roxlu/3d/Triangle.h>
#include <roxlu/3d/Quad.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/math/Vec4.h>
#include <roxlu/math/Vec2.h>


#include <iostream>
#include <vector>
#include <string>
#include <map>

using std::endl;
using std::ostream;
using std::vector;
using std::string;
using std::map;

namespace roxlu {

class VertexData {
public:
	VertexData(); 
	VertexData(const string& meshName);
	size_t 			addVertex(const Vec3& vec);
	size_t 			addVertex(const float x, const float y, const float z);
	vector<int>		addVertices(const vector<Vec3>& copy);
	size_t 			addTexCoord(const Vec2& vec);
	size_t			addTexCoord(const float x, const float y);
	size_t 			addColor(const Vec3& color);
	size_t 			addColor(const Color4& color);
	size_t 			addColor(const float r, const float g, const float b);
	size_t			addNormal(const float x, const float y, const float z);
	size_t			addTangent(const Vec3& tangent);
	size_t			addBinormal(const Vec3& binorm);
	size_t 			addNormal(const Vec3& vec);
	size_t			addTriangle(Triangle t);
	size_t 			addTriangle(int a, int b, int c); 
	size_t			addTriangleAndIndices(int a, int b, int c);
	size_t 			addQuad(int a, int b, int c, int d);
	size_t			addQuad(Quad q);
	size_t			addQuadAndIndices(int a, int b, int c, int d);
	void			addQuadIndices(int a, int b, int c, int d);
	void 			addIndex(const int& index);	
	void			addToVertexGroup(const string& name, const int& vertexIndex);

	void debugDraw(int drawMode = GL_TRIANGLES);
	void debugDrawQuad(int quadNum);
	
	const float* 	getVerticesPtr();
	const float* 	getTexCoordsPtr();
	const float* 	getColorsPtr();
	const float* 	getNormalsPtr();
	const int* 		getIndicesPtr();

	Triangle&		getTriangle(int index);
    Triangle* 		getTrianglePtr(int index);
	Quad* 			getQuadPtr(int quad);
	

	size_t 			getNumVertices();
	size_t 			getNumTexCoords();
	size_t 			getNumColors();
	size_t 			getNumNormals();
	size_t			getNumTangents();
	size_t 			getNumIndices();
	size_t 			getNumTriangles();
	size_t 			getNumQuads();
	size_t			getNumVertexGroups();
	
	Vec2			getTexCoord(int index);
	Vec2*			getTexCoordPtr(int index);
	Vec3 			getVertex(int index);
	Vec3*			getVertexPtr(int index);
	Vec3&			getVertexRef(int index);		
	Vec3			getNormal(int index);
	Vec3*			getNormalPtr(int index);
	Vec3&			getNormalRef(int index);
	
	VertexP* 		getVertexP();  	
	VertexPT*		getVertexPT();
	VertexPN* 		getVertexPN();
	VertexPTN* 		getVertexPTN();
	VertexPTNT*		getVertexPTNT();
	VertexPNC* 		getVertexPNC();
	VertexPTNTB* 	getVertexPTNTB();
	
	bool			getVertexGroup(const string& name, vector<int>& result);
	
	void			clearAttribs();
	void			enablePositionAttrib();
	void			enableNormalAttrib();
	void			enableColorAttrib();
	void			enableTexCoordAttrib();
	void			disablePositionAttrib();
	void			disableNormalAttrib();
	void			disableColorAttrib();
	void			disableTexCoordAttrib();

	void			clear();
	
	void		setNormal(int dx, Vec3 normal) { normals[dx] = normal; }
	void 		setVertex(int dx, Vec3 position) { vertices[dx] = position; }
	void 		setVertex(int dx, float x, float y, float z) { vertices[dx].set(x, y,z); }
	void 		setName(string n);
	string		getName();
	
	friend ostream& operator <<(ostream& os, const VertexData& data);
	Vec3& operator[](const unsigned int dx);
	Vec3& at(const unsigned int dx);
	const size_t size() const; // num vertices 
		
	Vec3			computeQuadNormal(int nQuad);
	
	// tangent compute: experimental
	void 			computeTangents();
	void			computeTangentForTriangle(Vec3& v1, Vec3& v2, Vec3& v3, Vec2& w1, Vec2& w2, Vec2& w3, Vec3& sdir, Vec3& tdir);
	void 			createTangentAndBiTangent(Vec3 va, Vec3 vb, Vec2 ta, Vec2 tb, Vec3& normal, Vec3& out_tangent, Vec3& out_bitangent);
	
	// debug
	void print();
	void printVertices();
	void printNormals();
	void printTangents();
	void printBinormals();
	void printTexCoords();
	void printTriangles();
	
	vector<Vec3>		normals;
	vector<Vec3>		vertices;
	vector<Vec2>		texcoords;
	vector<Color4> 		colors;
	vector<int> 		indices;
	vector<Triangle> 	triangles; 
	vector<Quad> 		quads;
	vector<Vec3>		tangents;
	vector<Vec3>		binormals;
	map<string, vector<int> >	vertex_groups; // name, vertex index for vertex groups
	
	int 				attribs;
	VertexP* 			vertex_p;
	VertexPTN* 			vertex_ptn;
	VertexPTNT*			vertex_ptnt;
	VertexPNC* 			vertex_pnc;
	VertexPN* 			vertex_pn;
	VertexPT*			vertex_pt;
	VertexPTNTB* 		vertex_ptntb;
	string 				name;
	
	static int			num_instances;
};



// Search for a vertex data by name:
// vector<VertexData*>::iterator it = std::find_if(vertex_datas.begin(), vertex_datas.end(), CompareVertexDataByName(name));
class CompareVertexDataByName {
public:
	CompareVertexDataByName(string name):name(name) {}
	bool const operator()(const VertexData* other) const {
		return name == other->name;
	}
	const string name;
};

inline const size_t VertexData::size() const {
	return vertices.size();
}

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
inline void VertexData::setName(string n) {
	name = n;
}

inline string VertexData::getName() {
	return name;
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

// you can use the index operator to access vertices.
inline Vec3& VertexData::operator[](const unsigned int dx) {
	return vertices[dx];
}
inline Vec3& VertexData::at(const unsigned int dx) {
	return vertices.at(dx);
}

} // roxlu

#endif