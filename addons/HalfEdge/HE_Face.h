#ifndef HE_FACEH
#define HE_FACEH

#include "HE_Headers.h"
#include <set>
using std::set;

namespace roxlu {

class Ray;
class Vec3;

class HE_Vertex;
class HE_HalfEdge;
class HE_Edge;

class HE_Face {
public:
	HE_Face();
	HE_Face(HE_HalfEdge* e);
	Vec3 getNormal();
	Vec3 getCenter();
	inline void setHalfEdge(HE_HalfEdge* e);
	inline HE_HalfEdge* getHalfEdge();

	int getNumVertices();
	bool intersectsWithRay(const Ray& r);

	const set<HE_Vertex*> getVertices();
	const set<HE_Edge*> getEdges();
	const set<HE_HalfEdge*> getHalfEdges();
	void sort();
	
private:
	HE_HalfEdge* half_edge;
	bool is_sorted;
};

inline HE_HalfEdge* HE_Face::getHalfEdge() {
	return half_edge;
}


inline void HE_Face::setHalfEdge(HE_HalfEdge* e) {
	half_edge = e;
}

}; // roxlu
#endif