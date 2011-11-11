#include "SceneItem.h"
#include "OBJ.h"
#include "File.h"
#include "HalfEdge.h"
namespace roxlu {

OBJ::OBJ() {
}

OBJ::~OBJ() {
}

void OBJ::exportSceneItem(SceneItem* si) {
	return exportSceneItem(*si);
}

void OBJ::exportSceneItem(SceneItem& si) {
	stringstream ss;
	ss << "# Roxlu OBJ export 0.01" << endl;	
	ss << "o " << si.getName() << endl;
	ss.precision(5);
	ss.setf(ios::fixed,ios::floatfield);

	VertexData& vd = *si.getVertexData();		
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		Vec3& v = *vd.getVertexPtr(i);
		ss << "v " << v.x << " " << v.y << " " << v.z << endl;
	}
	for(int i = 0; i < vd.getNumQuads(); ++i) {
		Quad& q = *vd.getQuadPtr(i);
		printf("%d, %d, %d, %d <---\n", q.a, q.b, q.c, q.d);
		ss << "f " << q.d+1 << " " << q.c+1 << " " << q.b+1 << endl;
		ss << "f " << q.b+1 << " " << q.a+1 << " " << q.d+1 << endl;
	}
	for(int i = 0; i < vd.getNumTriangles(); ++i) {
		Triangle& t = *vd.getTrianglePtr(i);
		ss << "f " << t.a+1 << " " << t.b+1 << " " << t.c+1 << endl;

	}
	
	// create unique list with vertices.
	/*
	vector<Vec2> unique_texcoords;
	vector<Vec3> unique_vertices;
	VertexData& vd = *si.getVertexData();	
	for(int i = 0; i < vd.getNumVertices(); ++i) {
		bool found = false;
		Vec3& a = *vd.getVertexPtr(i);
		int dx = 0;
		for(int j = 0; j < unique_vertices.size(); ++j) {
			Vec3 b = unique_vertices[j];
			if(b.lengthSquared(a) < 0.01) {
				dx = j;
				found = true;
				break;
			}
		}
		if(!found) {
			unique_vertices.push_back(a);
			
			// copy texcoord
			if(vd.getNumTexCoords() > 0) {
				unique_texcoords.push_back(vd.getTexCoord(i));
			}
		}
	}

	// write vertices
	for(int i = 0; i < unique_vertices.size(); ++i) {
		Vec3& v = unique_vertices[i];
		ss << "v " << v.x << " " << v.y << " " << v.z << endl;
	}
	
	// now for face find the index from the unique vertices.
	
	int num_quads = vd.getNumQuads();
	if(num_quads > 0) {
		vector<Quad> new_quads;
		for(int i = 0; i < vd.getNumQuads(); ++i) {
			Quad new_quad;
			Quad& q = *vd.getQuadPtr(i);
			Vec3 v;
			// = vd.getVertex(q.a);
			for(int k = 0; k < 4; ++k) {
				v = vd.getVertex(q[k]);
				for(int j = 0; j < unique_vertices.size(); ++j) {
					if(v.lengthSquared(unique_vertices[j]) < 0.01) {
						printf("%d wordt %d\n", q[k], j);
						new_quad[k] = j;
						break;
					}
				}
			}
			new_quads.push_back(new_quad);
		}
	
		for(int i = 0; i < new_quads.size(); ++i) {
			Quad& q = new_quads[i];
			ss << "f " << q.d+1 << " " << q.c+1 << " " << q.b+1 << endl;
			ss << "f " << q.b+1 << " " << q.a+1 << " " << q.d+1 << endl;
		}

		printf("We have: %d  unique vertices\n", unique_vertices.size());		
	}
	
	*/
	
			

		
	// And write everything to a file.
	File::writeToFile(File::toDataPath("test2.obj"), ss.str());

}



}; // roxlu