#ifndef HEC_FROMTRIANGLESH
#define HEC_FROMTRIANGLESH

#include "HEC_Creator.h"
#include "HEC_FromFaceList.h"
#include "Vec3.h"
#include "Triangle.h"
#include <vector>
//#include "ofMain.h" // tmp

using std::vector;

namespace roxlu {

class HE_Mesh;

class HEC_FromTriangles : public HEC_Creator {
public:
	HEC_FromTriangles();
	
	void addVertex(float x, float y, float z);
	void addTriangle(int a, int b, int c);
	
	bool create(HE_Mesh& mesh);
private:
	HEC_FromFaceList from_face_list;
};

inline void HEC_FromTriangles::addVertex(float x, float y, float z) {
	from_face_list.addVertex(x,y,z);
}

inline void HEC_FromTriangles::addTriangle(int a, int b, int c) {
	from_face_list.addTriangle(a,b,c);
}

}; // roxlu
#endif