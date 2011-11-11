#include "HEC_FromFaceList.h"
#include "HE_Mesh.h"

namespace roxlu {

HEC_FromFaceList::HEC_FromFaceList() 
:HEC_Creator(){
}

bool HEC_FromFaceList::create(HE_Mesh& mesh)  {
	// add vertices
	vector<HE_Vertex*> created_vertices;
	vector<Vec3>::iterator vec_it = vertices.begin();
	int i = 0; 
	while(vec_it != vertices.end()) {	
		HE_Vertex* vert = new HE_Vertex(*vec_it);
		vert->setLabel(i);
		mesh.addVertex(vert);
		created_vertices.push_back(vert);
		++vec_it;
		++i;
	}
		
	// create edges.
	vector<vector<int> >::iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		// get the indices for this face.
		vector<int>& indices = *face_it;
		int num_indices	= indices.size();
		if(num_indices < 2) {
			++face_it;
			continue;
		}
		
		// create a new face and it's edges.
		HE_Face* f = new HE_Face();
		vector<HE_HalfEdge*> face_half_edges;
		
		for(int i = 0; i < num_indices; ++i) {
			HE_HalfEdge* he = new HE_HalfEdge();
			he->setVertex(created_vertices.at(indices.at(i)));
			he->getVertex()->setHalfEdge(he);
			
			if(f->getHalfEdge() == NULL) {
				f->setHalfEdge(he);
			}
			
			// keep track of created edges so we can link them.
			face_half_edges.push_back(he);
		}
		
		// set next/prev on created half edges.
		mesh.addFace(f);
		mesh.addHalfEdges(face_half_edges);
		HE_Mesh::cycleHalfEdges(face_half_edges); // link half edges
		++face_it;
	}
	mesh.pairHalfEdges();
	mesh.capHalfEdges(); // finalize
	return true;
}

}; // roxlu