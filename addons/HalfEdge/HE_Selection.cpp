#include "HE_Selection.h"
#include "HE_Edge.h"
#include "HE_Vertex.h"
#include "HE_HalfEdge.h"
#include "HE_Mesh.h"

#include <algorithm>
#include <set>
using std::set;
using std::inserter;

namespace roxlu {

HE_Selection::HE_Selection(HE_Mesh* m)
	:parent(m)
{
}

// collect vertices from selected edges, faces, etc...
void HE_Selection::collectVertices() {
	set<HE_Vertex*> unique_vertices;
	
	// collect from faces.
	vector<HE_Face*>::const_iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		const set<HE_Vertex*> face_vertices = (*face_it)->getVertices();
		copy(face_vertices.begin(), face_vertices.end(), inserter(unique_vertices, unique_vertices.begin()));
		++face_it;
	}

	// collect from edges (@todo TEST THIS PIECE, NOT TESTED/VALIDATED)
	vector<HE_Edge*>::const_iterator edge_it = edges.begin();
	while(edge_it != edges.end()) {
		unique_vertices.insert((*edge_it)->getStartVertex());
		unique_vertices.insert((*edge_it)->getEndVertex());
		++edge_it;
	}
	
	// collect from half edges (@todo TEST THIS PIECE, NOT TESTED/VALIDATED)
	vector<HE_HalfEdge*>::const_iterator hedge_it = half_edges.begin();
	while(hedge_it != half_edges.end()) {
		unique_vertices.insert((*hedge_it)->getVertex());
		++hedge_it;
	}
	
	// now copy all unique vertices
	copy(unique_vertices.begin(), unique_vertices.end(), back_inserter(vertices));
	//cout << "Added vertices:" << vertices.size() << endl;
			
	/* validate 
	// tmp
	face_it = faces.begin();
	while(face_it != faces.end()) {
		vector<HE_Vertex*> face_vertices = (*face_it)->getVertices();
		for(int i = 0; i < face_vertices.size(); ++i) {
			cout << "orig: " << face_vertices[i] << endl;
		}
		++face_it;
	}
	cout << unique_vertices.size() << "<------" << endl;
	set<HE_Vertex*>::iterator it = unique_vertices.begin();
	while(it != unique_vertices.end()) {
		cout << "unique: " << *it << endl;
		++it;
	}
	// end tmp
	*/
	
}

void HE_Selection::collectFaces() {
	/*
	set<HE_Edge*> unique_edges;
	vector<HE_Face*>::const_iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		
		++face_it;
	}
	*/
}

void HE_Selection::collectAll(HE_Mesh* mesh) {
	clear();
	addFaces(mesh->getFacesRef());
	addEdges(mesh->getEdgesRef());
	addHalfEdges(mesh->getHalfEdgesRef());
	addVertices(mesh->getVerticesRef());
}

void HE_Selection::collectEdges() {
	set<HE_Edge*> unique_edges;
	vector<HE_Face*>::const_iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		const set<HE_Edge*> edges = (*face_it)->getEdges();
		copy(edges.begin(), edges.end(), inserter(unique_edges, unique_edges.begin()));
		++face_it;
	}
	copy(unique_edges.begin(), unique_edges.end(), back_inserter(edges));
}

void HE_Selection::collectHalfEdges() {
}

vector<HE_Edge*> HE_Selection::getOuterEdges() {
	HE_Selection copy = *this;
	copy.collectEdges();
	const vector<HE_Edge*>& copy_edges = copy.getEdgesRef();
	vector<HE_Edge*>::const_iterator it = copy_edges.begin();
	HE_Edge* e = NULL;
	HE_Face* f1 = NULL;
	HE_Face* f2 = NULL;
	
	vector<HE_Edge*> result;
	while(it != copy_edges.end()) {
		e = *it;
		f1 = e->getFirstFace();
		f2 = e->getSecondFace();
		
		if( (f1 == NULL) 
			|| (f2 == NULL) 
			|| (!containsFace(f1)) 
			|| (!containsFace(f2)) )
		{
			result.push_back(e);
		}
		++it;
	}
	return result;
}

vector<HE_Edge*> HE_Selection::getInnerEdges() {
	HE_Selection copy = *this;
	copy.collectEdges();
	const vector<HE_Edge*>& copy_edges = copy.getEdgesRef();
	vector<HE_Edge*>::const_iterator it = copy_edges.begin();
	HE_Edge* e = NULL;
	HE_Face* f1 = NULL;
	HE_Face* f2 = NULL;
	
	vector<HE_Edge*> result;
	while(it != copy_edges.end()) {
		e = *it;
		f1 = e->getFirstFace();
		f2 = e->getSecondFace();

		if( (f1 != NULL) 
			&& (f2 != NULL) 
			&& (containsFace(f1)) 
			&& (containsFace(f2)) ) 
		{
			result.push_back(e);
		}
		++it;
	}
	return result;

}

set<HE_Vertex*> HE_Selection::getOuterVertices() {
	set<HE_Vertex*> result;
	vector<HE_Edge*> outer_edges = getOuterEdges();
	vector<HE_Edge*>::iterator it = outer_edges.begin();
	while(it != outer_edges.end()) {
		HE_Edge* e = (*it);
		HE_Vertex* v1 = e->getStartVertex();
		HE_Vertex* v2 = e->getEndVertex();
		result.insert(v1);
		result.insert(v2);
		++it;
	}
	return result;
}

set<HE_Vertex*> HE_Selection::getInnerVertices() {
	HE_Selection sel = *this;
	sel.collectVertices();
	set<HE_Vertex*> result;
	set<HE_Vertex*> outer_verts = getOuterVertices();
	const vector<HE_Vertex*>& all_vertices = sel.getVerticesRef();
	vector<HE_Vertex*>::const_iterator it = all_vertices.begin();
	HE_Vertex* v;
	while(it != all_vertices.end()) {
		if(outer_verts.find(*it) == outer_verts.end()) {
			result.insert(*it);
		}
		++it;
	}
	return result;
}
// Operators..
// -----------------------------------------------------------------------------
HE_Selection& HE_Selection::operator+=(const HE_Selection& other) {
	addFaces(other.getFacesRef());
	addVertices(other.getVerticesRef());
	addEdges(other.getEdgesRef());
	addHalfEdges(other.getHalfEdgesRef());
	return *this;
}

HE_Selection HE_Selection::operator+(const HE_Selection& other) const {
	HE_Selection sel(parent);
	sel.addFaces(getFacesRef());
	sel.addVertices(getVerticesRef());
	sel.addEdges(getEdgesRef());
	sel.addHalfEdges(getHalfEdgesRef());
	sel += *this;
	return sel;
}

HE_Selection HE_Selection::operator=(const HE_Selection& other) const {
	HE_Selection sel(parent);
	sel.addFaces(getFacesRef());
	sel.addVertices(getVerticesRef());
	sel.addEdges(getEdgesRef());
	sel.addHalfEdges(getHalfEdgesRef());
	return sel;
}

}; // roxlu