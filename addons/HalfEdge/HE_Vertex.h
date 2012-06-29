#ifndef HE_VERTEXH
#define HE_VERTEXH

#include <set>
#include "Vec2.h"
#include "Vec3.h"

using std::set;

namespace roxlu {

class HE_HalfEdge;

class HE_Vertex {
public:	
	HE_Vertex();
	HE_Vertex(Vec3 pos);
	HE_Vertex(float x, float y, float z);
	~HE_Vertex();
	inline Vec3* getPositionPtr();
	inline Vec3& getPositionRef();
	inline void setTexCoord(float u, float v);
	inline void setHalfEdge(HE_HalfEdge* he);
	inline HE_HalfEdge* getHalfEdge();
	inline void setLabel(int l);
	inline int getLabel();
	set<HE_Vertex*> getNeighborVertices();

private:
	Vec3 position; // no, we do not use pointer for this.. to much of a hassle!
//	Vec3* position; // we use a pointer so you can easily change the position with ie a particle system.
	Vec2 texcoord; // texcoords won't change by external systems, so we use a normal var.
	HE_HalfEdge* half_edge;
	int label;
};

inline Vec3* HE_Vertex::getPositionPtr() {
	return &position;
}

inline Vec3& HE_Vertex::getPositionRef() {
	return position;
}

inline void HE_Vertex::setTexCoord(float u, float v) {
	texcoord.set(u,v);
}

inline void HE_Vertex::setHalfEdge(HE_HalfEdge* he) {
	half_edge = he;
}

inline HE_HalfEdge* HE_Vertex::getHalfEdge() {
	return half_edge;
}

inline void HE_Vertex::setLabel(int l) {
	label = l;
}

inline int HE_Vertex::getLabel() {
	return label;
}

}; // roxlu

#endif