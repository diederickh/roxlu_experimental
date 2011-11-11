#ifndef HEC_FROMFACELISTH
#define HEC_FROMFACELISTH

#include <vector>
#include <algorithm>
#include "Vec3.h"
#include "HEC_Creator.h"
#include "ofMain.h" // tmp
using std::vector;

namespace roxlu {

class HE_Mesh;

class HEC_FromFaceList  : public HEC_Creator {
public:
	HEC_FromFaceList();

	bool create(HE_Mesh& mesh);
	inline void addVertices(const vector<Vec3>& vertices);
	inline void addFaces(const vector<vector<int> >& faces);
		
	inline void addVertex(Vec3 v);
	inline void addVertex(float x, float y, float z);
	inline void addTriangle(int a, int b, int c);
	inline void addQuad(int a, int b, int c, int d);

private:
	vector<vector<int> > faces;
	vector<Vec3> vertices;
};

inline void HEC_FromFaceList::addVertex(Vec3 v) {
	vertices.push_back(v);
}

inline void HEC_FromFaceList::addVertex(float x, float y, float z) {
	vertices.push_back(Vec3(x,y,z));
}

inline void HEC_FromFaceList::addVertices(const vector<Vec3>& verts) {
	copy(verts.begin(), verts.end(), std::back_inserter(vertices));
}

inline void HEC_FromFaceList::addFaces(const vector<vector<int> >& fd) {
	copy(fd.begin(), fd.end(), std::back_inserter(faces));
}

inline void HEC_FromFaceList::addTriangle(int a, int b, int c) {
	vector<int> fd;
	fd.push_back(a);
	fd.push_back(b);
	fd.push_back(c);
	faces.push_back(fd);
}

inline void HEC_FromFaceList::addQuad(int a, int b, int c, int d) {
	vector<int> fd;
	fd.push_back(a);
	fd.push_back(b);
	fd.push_back(c);
	fd.push_back(d);
	faces.push_back(fd);
}

}; // roxlu
#endif