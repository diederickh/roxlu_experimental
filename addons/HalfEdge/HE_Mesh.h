#ifndef HE_MESHH
#define HE_MESHH

#include "HE_Vertex.h"
#include "HE_Structure.h"
#include "HE_HalfEdge.h"
#include "HE_Edge.h"
#include "HE_Face.h"
#include "HE_Debug.h"
#include "HE_Selection.h"


#include "HEC_Creator.h"
#include "HEC_FromTriangles.h"
#include "HEC_Plane.h"

#include "HEM_Modifier.h"
#include "HEM_Bend.h"
#include "HEM_Noise.h"

#include "HES_Subdividor.h"
#include "HES_CatmullClark.h"

#include "Vec3.h"
#include "Ray.h"

namespace roxlu {

class HE_Mesh : public HE_Structure {
public:
	HE_Mesh();
	~HE_Mesh(); 
	void drawHalfEdges();
	void drawFaces();
	void drawVertices();
	
	void pairHalfEdges();
	static void cycleHalfEdges(const vector<HE_HalfEdge*>& edges);
	void capHalfEdges();
	vector<HE_HalfEdge*> getUnpairedHalfEdges();
	
	HE_Selection quadSplitFaces();
	HE_Face* getRayFaceIntersection(const Ray& r);
	HE_Selection splitEdges();
	HE_Selection splitEdge(HE_Edge* e, float where); // 0-1, 0.5 = center
	HE_Selection splitEdge(HE_Edge* e, Vec3 p);
	inline HE_Selection selectAllFaces();
	
	inline HE_Mesh& subdivide(HES_Subdividor& subdiv, int num = 1);
	inline void resetVertexLabels();
};

/**
 * Use this method only to set the "next" half edges for the given 
 * vector. Note that this are just the half edges of a face. So the 
 * size of the vector will be around 3,4,5 (or maybe bigger for 
 * polygons.
 *
 * But make sure you only pass vector for one face!
 *
 */
inline void HE_Mesh::cycleHalfEdges(const vector<HE_HalfEdge*>& edges) {
	vector<HE_HalfEdge*>::const_iterator it = edges.begin();
	while(it != edges.end()) {
		(*it)->setNext(*(it+1));
		++it;
	}
	edges.back()->setNext(edges.front());
}

/**
 * This method will check which face intersects with the given 
 * ray. For now we only check triangles.
 *
 */
inline HE_Face* HE_Mesh::getRayFaceIntersection(const Ray& r) {
	vector<HE_Face*>::iterator it = faces.begin();
	while(it != faces.end()) {
		HE_Face& f = *(*it);
		if(f.intersectsWithRay(r)) {
			return *it;
		}
		++it;
	}
	return NULL;
}


// Get a selection from all faces.
inline HE_Selection HE_Mesh::selectAllFaces() {
	HE_Selection sel(this);
	sel.addFaces(getFacesRef());
	return sel;
}

// subdivide
inline HE_Mesh& HE_Mesh::subdivide(HES_Subdividor& subdiv, int num) {
	for(int i = 0; i < num; ++i) {
		subdiv.apply(*this);	
	}
	return *this;
}

// reset vertex labels (i.e. used in HES_CatmullClark)
inline void HE_Mesh::resetVertexLabels() {
	vector<HE_Vertex*>::iterator it = vertices.begin();
	while(it != vertices.end()) {
		(*it)->setLabel(-1);
		++it;
	}
}

}; // roxlu

#endif
