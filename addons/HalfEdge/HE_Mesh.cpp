#include "HE_Mesh.h"
namespace roxlu {

HE_Mesh::HE_Mesh() 	
	:HE_Structure() 
{
}


// @todo: test if we cleanup memory correctly!!
HE_Mesh::~HE_Mesh() {
	// delete faces.
	vector<HE_Face*>::iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		delete *face_it;
		++face_it;
	}
	
	// delete edges
	vector<HE_Edge*>::iterator edge_it = edges.begin();
	while(edge_it != edges.end()) {
		delete *edge_it;
		++edge_it;
	}
	
	// delete half edges
	vector<HE_HalfEdge*>::iterator hedge_it = half_edges.begin();
	while(hedge_it != half_edges.end()) {
		delete *hedge_it;
		++hedge_it;
	}
	
	// delete vertices
	vector<HE_Vertex*>::iterator v_it = vertices.begin();
	while(v_it != vertices.end()) {
		delete *v_it;
		++v_it;
	}
}

void HE_Mesh::drawHalfEdges() {
}

void HE_Mesh::drawFaces() {
}

void HE_Mesh::drawVertices() {
}


// TODO test
vector<HE_HalfEdge*> HE_Mesh::getUnpairedHalfEdges() {
	vector<HE_HalfEdge*>::iterator it = half_edges.begin();
	vector<HE_HalfEdge*> result;
	while(it != half_edges.end()) {
		if((*it)->getPair() == NULL) {
			result.push_back((*it));
		}
		++it;
	}
	return result;
}

void HE_Mesh::pairHalfEdges() {
	vector<HE_HalfEdge*> unpaired = getUnpairedHalfEdges();
	vector<HE_HalfEdge*>::iterator it = unpaired.begin();
	vector<HE_HalfEdge*>::iterator other_it;
	
	try {
		while(it != unpaired.end()) {
			HE_HalfEdge& he_a = *(*it);
			if(he_a.getPair() == NULL) {
				other_it =(it+1);

				while(other_it != unpaired.end()) {
					HE_HalfEdge& he_b = *(*other_it);

					// some trickery to check if the other is a pair.
					if(	(he_b.getPair() == NULL) 
						&& (he_a.getVertex() == he_b.getNext()->getVertex()) 
						&& (he_b.getVertex() == he_a.getNext()->getVertex()) 
					)
					{
						he_a.setPair(&he_b);
						
						// create a new edge for both half edges
						HE_Edge* e = new HE_Edge();
						e->setHalfEdge(&he_a);
						he_a.setEdge(e);
						he_b.setEdge(e);
						addEdge(e);
					}
					
					++other_it;
				}
			}
			++it;
		}
	}
	catch(...) {
		cout << "Oops something went wrong when pairing half edges." << endl;
	}
}

// cap = aftoppen
void HE_Mesh::capHalfEdges() {
	vector<HE_HalfEdge*> new_half_edges;
	vector<HE_HalfEdge*> unpaired_half_edges = getUnpairedHalfEdges();
	
	for(int i = 0; i < unpaired_half_edges.size(); ++i) {
		// create a new half edge for the unpaired one.
		HE_HalfEdge* he0 = unpaired_half_edges.at(i);
		HE_HalfEdge* he1 = new HE_HalfEdge();
		addHalfEdge(he1);
		
		// set the vertex for the new half edge.
		he1->setVertex(he0->getNext()->getVertex());
		he0->setPair(he1);
		
		// create a new edge for both half edge
		HE_Edge* e = new HE_Edge();
		addEdge(e);
		e->setHalfEdge(he0);
		he0->setEdge(e);
		he1->setEdge(e);
		
		// keep track of new half edges.
		new_half_edges.push_back(he1);
	}
	
	// set next pointers
	int n = new_half_edges.size();
	for(int i = 0; i < n; ++i) {
		HE_HalfEdge* he0 = new_half_edges.at(i);
		if(he0->getNext() == NULL) {
			for(int j = 0; j < n; ++j) {
				HE_HalfEdge* he1 = new_half_edges.at(j);
				if(he1->getVertex() == he0->getPair()->getVertex()) {
					he0->setNext(he1);
					break;
				}		
			}
		}
	}
}



HE_Selection HE_Mesh::quadSplitFaces() {
		
	// Collect the number of vertices per face and face centers.
	vector<Vec3> face_centers;
	vector<int> face_sizes;
	vector<HE_Face*>::const_iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		HE_Face& face = *(*face_it);
		Vec3 face_center = face.getCenter();
		face_centers.push_back(face_center);
		face_sizes.push_back(face.getNumVertices());
		++face_it;
	}

	// Collect current vertices, edges, halfedges.
	int num_faces = getNumFaces();
	HE_Selection sel_orig(this);
	sel_orig.addFaces(getFaces());
	sel_orig.collectVertices();
	sel_orig.collectEdges();
	
	
	// split edges
	HE_Selection sel_out(this);
	sel_out.addVertices(splitEdges().getVerticesRef());
	
	// create new quad faces
	vector<HE_Face*> faces_copy = getFaces();
	for(int i = 0; i < face_centers.size(); ++i) {
		// create new vertex in the center of the faces.
		HE_Vertex* vi = new HE_Vertex(face_centers[i]);
		vi->setLabel(2);
		addVertex(vi);
		sel_out.addVertex(vi);
				
		HE_Face* f = faces_copy[i];
		HE_HalfEdge* start_he = f->getHalfEdge();
		while(sel_orig.containsVertex(start_he->getVertex())) {
			start_he = start_he->getNext();
		}
		HE_HalfEdge* he = start_he;
		
		// perform the quad split
		vector<HE_HalfEdge*> he0;
		vector<HE_HalfEdge*> he1;
		vector<HE_HalfEdge*> he2;
		vector<HE_HalfEdge*> he3;
		he0.reserve(face_sizes[i]);
		he1.reserve(face_sizes[i]);
		he2.reserve(face_sizes[i]);
		he3.reserve(face_sizes[i]);		
		
		int c = 0;
		do {
			HE_Face* fc = NULL;
			if(c == 0) {
				fc = f;
			}
			else {
				fc = new HE_Face();
				addFace(fc);
			}
			
			he0[c] = he;
			he->setFace(fc);
			fc->setHalfEdge(he);
			
			he1[c] = he->getNext();
			he2[c] = new HE_HalfEdge();
			he3[c] = new HE_HalfEdge();
			
			addHalfEdge(he2[c]);
			addHalfEdge(he3[c]);
			
			he2[c]->setVertex(he->getNext()->getNext()->getVertex());
			he3[c]->setVertex(vi);
			he2[c]->setNext(he3[c]);
			he3[c]->setNext(he);
			
			he1[c]->setFace(fc);
			he2[c]->setFace(fc);
			he3[c]->setFace(fc);
			c++;
			he = he->getNext()->getNext();

		} while(he != start_he);
		
			
		vi->setHalfEdge(he3[0]);
		for(int j = 0; j < c; ++j) {
			he1[j]->setNext(he2[j]);
		}
		
	}
	pairHalfEdges();
	return sel_out;
}



HE_Selection HE_Mesh::splitEdges() {
	HE_Selection sel_out(this);
	vector<HE_Edge*> edges_copy = getEdges();
	vector<HE_Edge*>::iterator edge_it = edges_copy.begin();
	while(edge_it != edges_copy.end()) {
		HE_Edge* e = *edge_it;
		sel_out += splitEdge(e, 0.5);		
		++edge_it;
	}
	return sel_out;
}

HE_Selection HE_Mesh::splitEdge(HE_Edge* e, float where) {
	Vec3 dir = e->getEndVertex()->getPositionRef() - e->getStartVertex()->getPositionRef();
	dir.scale(0.5);
	Vec3 p = e->getStartVertex()->getPositionRef() + dir;
	return splitEdge(e, p);
}

HE_Selection HE_Mesh::splitEdge(HE_Edge* e, Vec3 p) {
	HE_Selection sel_out(this);
	
	// Current half edges.
	HE_HalfEdge* he0 = e->getHalfEdge();
	HE_HalfEdge* he1 = he0->getPair();
	
	// create new vertex and half edge.
	HE_Vertex* v = new HE_Vertex(p);
	HE_HalfEdge* he0_new = new HE_HalfEdge();
	HE_HalfEdge* he1_new = new HE_HalfEdge();
	
	// new half edges get same new vertex as start point.
	he0_new->setVertex(v);
	he1_new->setVertex(v); 
	v->setHalfEdge(he0_new);
	
	// new ones replace current half edges.
	he0_new->setNext(he0->getNext());
	he1_new->setNext(he1->getNext());
	he0->setNext(he0_new);
	he1->setNext(he1_new);
	
	// pair new half edge
	he0->setPair(he1_new);
	he0_new->setPair(he1);

	// create new edge and reset old edge
	HE_Edge* edge_new = new HE_Edge();
	edge_new->setHalfEdge(he0_new);
	he1_new->setEdge(e);
	he1->setEdge(edge_new);
	he0_new->setEdge(edge_new);
	
	// reset faces.
	if(he0->getFace() != NULL) {
		he0_new->setFace(he0->getFace());
	}
	if(he1->getFace() != NULL) {
		he1_new->setFace(he1->getFace());
	}

	// add newly created vertex, hafl edges and edge.	
	v->setLabel(1);
	addVertex(v);
	addHalfEdge(he0_new);
	addHalfEdge(he1_new);
	addEdge(edge_new);
	
	// set new vertex and edge to selection.
	sel_out.addVertex(v);
	sel_out.addEdge(edge_new);
	return sel_out;
}

}; // roxlu