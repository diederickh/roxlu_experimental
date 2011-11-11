#include "HES_CatmullClark.h"
#include "HE_Mesh.h"
#include <map>

using std::map;
using std::pair;

namespace roxlu {

HES_CatmullClark::HES_CatmullClark() 
	:HES_Subdividor()
	,keep_boundary(true)
{
}

bool HES_CatmullClark::apply(HE_Mesh& mesh) {
	// reset labels so we apply catmull-clark on existing vertices
	mesh.resetVertexLabels();

	// Get avarage center for each vertex.
	const vector<HE_Vertex*>& vertices = mesh.getVerticesRef();
	vector<HE_Vertex*>::const_iterator vert_it = vertices.begin();
	map<HE_Vertex*, Vec3> avarage_centers;
	while(vert_it != vertices.end()) {
		HE_Vertex* vert = *vert_it;
		HE_HalfEdge* he = vert->getHalfEdge();
		Vec3 avarage_center(0);
		int c = 0;
		
		// get avarage for face, related to current vertex.
		do {
			if(he->getFace() != NULL) {
				avarage_center += he->getFace()->getCenter();
				
				++c;
			}
			he = he->getNextInVertex();
		} while(he != vert->getHalfEdge());
	
		// store the avarage face center for this vertex.
		avarage_center /= c;
		avarage_centers.insert(pair<HE_Vertex*, Vec3>(*vert_it, avarage_center));
		++vert_it;
	}
	
	mesh.quadSplitFaces(); // we split it once; so we're sure their quads.

	// get inner and outer vertices.
	HE_Selection all = mesh.selectAllFaces();
	set<HE_Vertex*> boundary = all.getOuterVertices();
	set<HE_Vertex*> inner = all.getInnerVertices();
	
	
	// Check inner vertices.
	set<HE_Vertex*>::iterator inner_it = inner.begin();
	set<HE_Vertex*> neighbors;
	map<HE_Vertex*, Vec3> new_positions;
	Vec3 p;

	while(inner_it != inner.end()) {
		HE_Vertex* v = (*inner_it);
		if(v->getLabel() == -1) {
			map<HE_Vertex*, Vec3>::iterator avc_it = avarage_centers.find(v);
			neighbors = v->getNeighborVertices();
			int num_neighbors = neighbors.size();
			float io = 1.0 / num_neighbors;

			if(avc_it != avarage_centers.end()) {
				p = avc_it->second;
			
				set<HE_Vertex*>::iterator nit = neighbors.begin();
				while(nit != neighbors.end()) {
					HE_Vertex* neigh_vert = *nit;
					Vec3& np = neigh_vert->getPositionRef();
					p.x += 2.0 * io * np.x;
					p.y += 2.0 * io * np.y;
					p.z += 2.0 * io * np.z;
					++nit;
				}
				Vec3 v_pos = v->getPositionRef();
				v_pos *= (num_neighbors - 3);
				p += v_pos; 
				p /= num_neighbors;
				new_positions.insert(pair<HE_Vertex*, Vec3>(v,p));
			}
		}
		else {
			p.set(0,0,0);
			neighbors = v->getNeighborVertices();
			bool edge_point = false;
			int num_neighbors = neighbors.size();
			set<HE_Vertex*>::iterator neigh_it = neighbors.begin();
			while(neigh_it != neighbors.end()) {
				HE_Vertex* neigh_vert = *neigh_it;
				p += neigh_vert->getPositionRef();
				if(neigh_vert->getLabel() == -1) {
					edge_point = true;
				}
				++neigh_it;
			}
			p /= num_neighbors;
			if(edge_point) {
				new_positions.insert(pair<HE_Vertex*, Vec3>(v, p));
			}
			else {
				new_positions.insert(pair<HE_Vertex*, Vec3>(v, v->getPositionRef()));
			}
		}
		++inner_it;
	}
	
	
	// check outer vertices.
	set<HE_Vertex*>::iterator boundary_it = boundary.begin();
	while(boundary_it != boundary.end()) {	
		HE_Vertex* v = *boundary_it;
		if(keep_boundary) {
			new_positions.insert(pair<HE_Vertex*, Vec3>(v, v->getPositionRef()));
		}
		else {
			p = v->getPositionRef();
			neighbors = v->getNeighborVertices();
			int num_neighbors = neighbors.size();
			set<HE_Vertex*>::iterator nit = neighbors.begin();
			double c = 1;
			int nc = 0;
			while(nit != neighbors.end()) {
				HE_Vertex* neigh_vert = *nit;
				if(boundary.find(neigh_vert) != neighbors.end()) {
					p += v->getPositionRef();
					c++;
					nc++;
				}
				++nit;
			}
			new_positions.insert(pair<HE_Vertex*, Vec3>(v, (nc > 1) ? p.getScaled(1.0/c) : v->getPositionRef()));
		}
		++boundary_it;
	}
	
	// set new positions (?)
	inner_it = inner.begin();
	map<HE_Vertex*, Vec3>::iterator found_it;
	while(inner_it != inner.end()) {
		HE_Vertex* v = *inner_it;
		found_it = new_positions.find(v);
		if(found_it != new_positions.end()) {
			v->getPositionRef() = found_it->second; 
		}
		++inner_it;
	}
	
	boundary_it = boundary.begin();
	while(boundary_it != boundary.end()) {	
		HE_Vertex* v = *boundary_it;
		found_it = new_positions.find(v);
		if(found_it != new_positions.end()) {
			v->getPositionRef() =  found_it->second;
		}
		++boundary_it;
	}
	
	return true;
}

}; // roxlu