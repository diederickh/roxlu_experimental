#ifndef ROXLU_CONVEX_HULL3DH
#define ROXLU_CONVEX_HULL3DH

#include <vector>
#include <string>

#ifdef PI
	#undef PI
#endif

#ifdef TWO_PI
	#undef TWO_PI
#endif

#ifdef HALF_PI
	#undef HALF_PI
#endif

#ifdef DEG_TO_RAD
	#undef DEG_TO_RAD
#endif

#ifdef RAD_TO_DEG
	#undef RAD_TO_DEG
#endif

#ifdef B0
	#undef B0 // somewhere from termios
#endif

#include "Wm5Core.h"
#include "Wm5Mathematics.h"
#include "Wm5Images.h"
#include "Wm5Imagics.h"


using std::vector;
using std::string;

namespace roxlu {

class ConvexHull3D {

public:

	ConvexHull3D();
	~ConvexHull3D();
	
	int addPoint(float *p);
	int addPoint(float x, float y, float z);
	void create();
	bool save(const string& filepath);
	bool load(const string& filepath);
	string toString();
	
	const Wm5::Vector3f* getVertices();
	const int* getIndices();
	
	size_t getNumpoints();
	size_t getNumVertices();
	size_t getNumIndices();
	size_t getNumTriangles();
	
	void useRealQueryType();
	
	// -----------------------
	void clear();
	
	size_t num_triangles;
	size_t num_indices;
	size_t num_vertices;
		
	vector<float> points;
	Wm5::Vector3f* vertices;
	const int* indices;
	Wm5::Query::Type query_type;
	Wm5::ConvexHullf* chull;	

};

inline int ConvexHull3D::addPoint(float* p) {
	return addPoint(*p, *(p+1), *(p+2));
}

inline int ConvexHull3D::addPoint(float x, float y, float z) {
//	printf("x: %f, y:%f, z:%f\n", x,y,z);
	points.push_back(x);
	points.push_back(y);
	points.push_back(z);
	return getNumpoints()-1;
}

inline const int* ConvexHull3D::getIndices() {
	return indices;
}

inline const Wm5::Vector3f* ConvexHull3D::getVertices() {
	return vertices;
}

inline size_t ConvexHull3D::getNumpoints() {
	return points.size()/3;
}

inline size_t ConvexHull3D::getNumVertices() {
	return num_vertices;
}

inline size_t ConvexHull3D::getNumIndices() {
	return num_indices;
}

inline size_t ConvexHull3D::getNumTriangles() {
	return num_triangles;
}

inline void ConvexHull3D::useRealQueryType() {
	query_type = Wm5::Query::QT_RATIONAL;
}



} // roxlu


#endif