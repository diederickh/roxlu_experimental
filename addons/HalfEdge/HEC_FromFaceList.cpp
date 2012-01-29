#include "HEC_FromFaceList.h"
#include "HE_Mesh.h"
#include "Quad.h"
#include "VertexData.h"

namespace roxlu {

HEC_FromFaceList::HEC_FromFaceList() 
:HEC_Creator(){
}

void HEC_FromFaceList::copyFromVertexData(VertexData& vd) {
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		addVertex(vd.getVertex(i));
	}
	for(int i = 0; i < vd.getNumQuads(); ++i) {
		Quad& q = *vd.getQuadPtr(i);
		addQuad(q.a, q.b, q.c, q.d);
	}
}

bool HEC_FromFaceList::create(HE_Mesh& mesh)  {
	// add vertices
	bool remove_duplicates = true;
	vector<HE_Vertex*> created_vertices;
	if(remove_duplicates) {
		vector<Vec3> unique_vertices;
		int c= 0;
		for(int i = 0; i < vertices.size(); ++i) {
			bool found = false;
			Vec3& a = vertices[i];
			for(int j = 0; j < unique_vertices.size(); ++j) {
				Vec3& b = unique_vertices[j];
				float dist = a.lengthSquared(b);
				if(dist < 0.01) {
					found = true;
					break;
				}
			}
			if(!found) {
				unique_vertices.push_back(vertices[i]);
			}	
		}
		
		
		
//		printf("vertex %d already found as %d in unique vertices\n", i, j);
		// fix face indices
		for(int k = 0; k < faces.size(); ++k) {
			vector<int> face = faces[k];
			for(int l = 0; l < face.size(); ++l) {
				Vec3 fv = vertices[face[l]];
				// find same vertex in unique vertices.
				bool found = false;
				for(int i = 0; i < unique_vertices.size(); ++i) {
					if(unique_vertices[i].lengthSquared(fv) < 0.01) {
						faces[k][l] = i;
						found = true;
						break;
 //						printf(">> %d\n", i);
					}
				}
				
				if(!found) {
					printf("=====================\n");
				}
//				vector<Vec3>::iterator it = std::find(unique_vertices.begin(), unique_vertices.end(), fv);
//				if(it != unique_vertices.end()) {
//					int dx = unique_vertices.end() - it;
//					
//					//	for(int i = 0; i < unique_vertices; ++i)
//					printf("x: %f, y:%f, z:%f index: %d\n", fv.x, fv.y, fv.z, dx);
//				}
			}
		}
//		return;
		vertices.clear();
		vertices = unique_vertices;
		printf("------ duplicates: %d  total:%d\n", c, (int)unique_vertices.size());
		printf("num faces: %d\n", (int)faces.size());
		for(int k = 0; k < faces.size(); ++k) {
			vector<int> face = faces[k];
			for(int l = 0; l < face.size(); ++l) {
				printf("Face: %d, index:%d\n", k, face[l]);
			}
		}
		//return;
	}
	printf("create he_vertices\n");
//	else {
		vector<Vec3>::iterator vec_it = vertices.begin();
		int i = 0; 
		while(vec_it != vertices.end()) {	
			printf("create he_vertice: %d\n", i);
			HE_Vertex* vert = new HE_Vertex(*vec_it);
			vert->setLabel(i);

			mesh.addVertex(vert);
			created_vertices.push_back(vert);
			++vec_it;
			++i;
		}
//	}

//	return;
	printf("Create edges\n");	
	// create edges.
	vector<vector<int> >::iterator face_it = faces.begin();
	while(face_it != faces.end()) {
		// get the indices for this face.
		vector<int>& indices = *face_it;
		printf("create edge\n");
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