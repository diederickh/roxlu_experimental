#ifndef HE_STRUCTUREH
#define HE_STRUCTUREH

#include "HE_Headers.h"

namespace roxlu {

class HE_Vertex;
class HE_HalfEdge;
class HE_Edge;
class HE_Face;

class HE_Structure {
public:
	HE_Structure();
	
	inline void addVertex(HE_Vertex* v);
	inline void addFace(HE_Face* f);
	inline void addHalfEdge(HE_HalfEdge* he);
	inline void addEdge(HE_Edge* e);
	
	inline void addVertices(const vector<HE_Vertex*>& verts);
	inline void addHalfEdges(const vector<HE_HalfEdge*>& edges);
	inline void addFaces(const vector<HE_Face*>& fac);
	inline void addEdges(const vector<HE_Edge*>& e);
	
	inline vector<HE_Vertex*> getVertices();
	inline vector<HE_HalfEdge*> getHalfEdges();
	inline vector<HE_Face*> getFaces();
	inline vector<HE_Edge*> getEdges();
	
	inline const vector<HE_Face*>& getFacesRef() const;
	inline const vector<HE_Vertex*>& getVerticesRef() const;
	inline const vector<HE_HalfEdge*>& getHalfEdgesRef() const;
	inline const vector<HE_Edge*>& getEdgesRef()const;
	
	inline int getNumVertices();
	inline int getNumHalfEdges();
	inline int getNumFaces();
	inline int getNumEdges();
	
	inline HE_Vertex* getVertex(int dx);
	inline HE_HalfEdge* getHalfEdge(int dx);
	inline HE_Face* getFace(int dx);
	inline HE_Edge* getEdge(int dx);
	
	inline bool containsVertex(HE_Vertex* v);
	inline bool containsEdge(HE_Edge* e);
	inline bool containsHalfEdge(HE_HalfEdge* he);
	inline bool containsFace(HE_Face* f);
	
	inline void clear();
protected:
	vector<HE_Vertex*> vertices;
	vector<HE_HalfEdge*> half_edges;
	vector<HE_Edge*> edges;
	vector<HE_Face*> faces;
};


// Add one element a time
// -----------------------------------------------------------------------------
inline void HE_Structure::addVertex(HE_Vertex* v) {
	vertices.push_back(v);
}

inline void HE_Structure::addFace(HE_Face* f) {
	faces.push_back(f);
}

inline void HE_Structure::addEdge(HE_Edge* e) {
	edges.push_back(e);
}

inline void HE_Structure::addHalfEdge(HE_HalfEdge* he) {
	half_edges.push_back(he);
}

// Add multiple elements at a time
// -----------------------------------------------------------------------------
inline void HE_Structure::addVertices(const vector<HE_Vertex*>& verts) {
	copy(verts.begin(), verts.end(), back_inserter(vertices));
}

inline void HE_Structure::addHalfEdges(const vector<HE_HalfEdge*>& edges) {
	copy(edges.begin(), edges.end(), back_inserter(half_edges));
}

inline void HE_Structure::addEdges(const vector<HE_Edge*>& es) {
	copy(es.begin(), es.end(), back_inserter(edges));
}

inline void HE_Structure::addFaces(const vector<HE_Face*>& fac) {
	copy(fac.begin(), fac.end(), back_inserter(faces));
}

// Retrieving elements
// -----------------------------------------------------------------------------
inline vector<HE_Vertex*> HE_Structure::getVertices() {
	return vertices;
}

inline vector<HE_HalfEdge*> HE_Structure::getHalfEdges() {
	return half_edges;
}

inline vector<HE_Face*> HE_Structure::getFaces() {
	return faces;
}

inline vector<HE_Edge*> HE_Structure::getEdges() {
	return edges;
}

// Stats
// -----------------------------------------------------------------------------
inline int HE_Structure::getNumVertices() {
	return vertices.size();
}

inline int HE_Structure::getNumFaces() {
	return faces.size();
}

inline int HE_Structure::getNumHalfEdges() {
	return half_edges.size();
}

inline int HE_Structure::getNumEdges() {
	return edges.size();
}

// Retrieving edges, vertices, faces
// -----------------------------------------------------------------------------
inline HE_Face* HE_Structure::getFace(int dx) {
	return faces.at(dx);
}

inline HE_Vertex* HE_Structure::getVertex(int dx) {
	return vertices.at(dx);
}

inline HE_HalfEdge* HE_Structure::getHalfEdge(int dx) {
	return half_edges.at(dx);
}

inline HE_Edge* HE_Structure::getEdge(int dx) {
	return edges.at(dx);
}

// Get references to containers
// -----------------------------------------------------------------------------
inline const vector<HE_Face*>& HE_Structure::getFacesRef() const {
	return faces;	
}

inline const vector<HE_Vertex*>& HE_Structure::getVerticesRef() const {
	return vertices;
}

inline const vector<HE_HalfEdge*>& HE_Structure::getHalfEdgesRef() const {
	return half_edges;
}

inline const vector<HE_Edge*>& HE_Structure::getEdgesRef() const {
	return edges;
}

// Search for elements
// -----------------------------------------------------------------------------
inline bool HE_Structure::containsVertex(HE_Vertex* v) {
	return find(vertices.begin(), vertices.end(), v) != vertices.end();
}

inline bool HE_Structure::containsEdge(HE_Edge* e) {
	return find(edges.begin(), edges.end(), e) != edges.end();
}

inline bool HE_Structure::containsHalfEdge(HE_HalfEdge* he) {
	return find(half_edges.begin(), half_edges.end(), he) != half_edges.end();
}

inline bool HE_Structure::containsFace(HE_Face* f) {
	return find(faces.begin(), faces.end(), f) != faces.end();
}


// Utils
// -----------------------------------------------------------------------------
inline void HE_Structure::clear() {
	vertices.clear();
	half_edges.clear();
	edges.clear();
	faces.clear();
}

}; // roxlu



#endif