#ifndef HE_EDGEH
#define HE_EDGEH

#include "HE_HalfEdge.h"

namespace roxlu {

class HE_Vertex;
class HE_Face;

class HE_Edge {
public:
	inline HE_Edge(HE_HalfEdge* he):half_edge(he) { }
	inline HE_Edge():half_edge(NULL){}
	
	inline HE_HalfEdge* getHalfEdge();
	inline void 		setHalfEdge(HE_HalfEdge*);
	inline void 		clearHalfEdge();
	inline HE_Vertex* 	getStartVertex();	
	inline HE_Vertex*	getEndVertex();	
	inline HE_Face* 	getFirstFace();
	inline HE_Face* 	getSecondFace();
	
private:
	HE_HalfEdge* half_edge;
};

inline void HE_Edge::setHalfEdge(HE_HalfEdge* he) {
	half_edge = he;
}

inline HE_HalfEdge* HE_Edge::getHalfEdge() {
	return half_edge;
}

inline void HE_Edge::clearHalfEdge() {
	half_edge = NULL;
}

inline HE_Vertex* HE_Edge::getStartVertex() {
	return half_edge->getVertex();
}

inline HE_Vertex* HE_Edge::getEndVertex() {
	return half_edge->getPair()->getVertex();
}

inline HE_Face* HE_Edge::getFirstFace() {
	return half_edge->getFace();
}

inline HE_Face* HE_Edge::getSecondFace() {
	return half_edge->getPair()->getFace();
}

}; // roxlu

#endif