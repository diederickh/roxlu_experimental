#ifndef HESELECTIONH
#define HESELECTIONH
#include <vector>
#include <set>

#include "HE_Structure.h"

using std::vector;
using std::set;

namespace roxlu {

class HE_Mesh;
class HE_Edge;
class HE_Vertex;
class HE_HalfEdge;

class HE_Selection : public HE_Structure {
public:
	HE_Selection(HE_Mesh* m);
	vector<HE_Edge*> getOuterEdges();
	vector<HE_Edge*> getInnerEdges();
	
	set<HE_Vertex*> getOuterVertices();
	set<HE_Vertex*> getInnerVertices();
	vector<HE_Vertex*> getBoundaryVertices();
	
	vector<HE_HalfEdge*> getOuterHalfEdges();
	vector<HE_HalfEdge*> getOuterHalfEdgesInside();
	vector<HE_HalfEdge*> getInnerHalfEdges();
	
	void collectAll(HE_Mesh* mesh); // collect all from mesh
	void collectVertices();
	void collectFaces();
	void collectEdges();
	void collectHalfEdges();
	
	HE_Mesh* parent;
	HE_Selection& operator+=(const HE_Selection& other);
	HE_Selection operator+(const HE_Selection& other) const;
	HE_Selection operator=(const HE_Selection& other) const;
};

}; // roxlu

#endif