#ifndef HE_HALFEDGEH
#define HE_HALFEDGEH

namespace roxlu {

class HE_Vertex;
class HE_Face;
class HE_Edge;

class HE_HalfEdge {
public:
	HE_HalfEdge();
	HE_HalfEdge(HE_Vertex* v);
	inline HE_HalfEdge& setNext(HE_HalfEdge* e);
	inline HE_HalfEdge& setPrev(HE_HalfEdge* e);
	inline HE_HalfEdge& setPair(HE_HalfEdge* e);
	inline HE_HalfEdge& setFace(HE_Face* f);
	inline HE_HalfEdge& setVertex(HE_Vertex* v);
	inline HE_HalfEdge& setEdge(HE_Edge* e);
	
	inline HE_HalfEdge* getNext(); // get next in same face
	inline HE_HalfEdge* getNextInVertex(); // get next starting from vertex
	inline HE_HalfEdge* getPrev();
	inline HE_HalfEdge* getPair();
	inline HE_Face*	 	getFace();
	inline HE_Vertex*	getVertex();
	inline HE_Edge*		getEdge();
	

	HE_Vertex* vertex;		
	HE_HalfEdge* next;
	HE_HalfEdge* prev;
	HE_HalfEdge* pair;
	HE_Edge* edge;
	HE_Face* face;
};


inline HE_HalfEdge& HE_HalfEdge::setNext(HE_HalfEdge* e) {
	next = e; 
	return *this;
}

inline HE_HalfEdge& HE_HalfEdge::setPrev(HE_HalfEdge* e) {
	prev = e;
	return *this;
}

inline HE_HalfEdge& HE_HalfEdge::setPair(HE_HalfEdge* e) {
	e->pair = this;
	pair = e;
}

inline HE_HalfEdge& HE_HalfEdge::setFace(HE_Face* f) {
	face = f;
	return * this;
}

inline HE_HalfEdge& HE_HalfEdge::setVertex(HE_Vertex* v) {
	vertex = v;
	return *this;
}

inline HE_HalfEdge&  HE_HalfEdge::setEdge(HE_Edge* e) {
	edge = e;
	return *this;
}

inline HE_HalfEdge* HE_HalfEdge::getNextInVertex() {
	if(pair == NULL) {
		return NULL;
	}
	return pair->getNext();
}

inline HE_HalfEdge* HE_HalfEdge::getNext() {
	return next;
}

inline HE_HalfEdge* HE_HalfEdge::getPair() {
	return pair;
}

inline HE_Vertex* HE_HalfEdge::getVertex() {
	return vertex;
}

inline HE_HalfEdge* HE_HalfEdge::getPrev() {
	return prev;
}

inline HE_Face* HE_HalfEdge::getFace() {
	return face;
}

inline HE_Edge* HE_HalfEdge::getEdge() {
	return edge;
}

}; // roxlu

#endif