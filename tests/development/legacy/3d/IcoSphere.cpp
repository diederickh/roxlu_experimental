//#include "IcoSphere.h"

//#include <roxlu/3d/VertexData.h>
#include <roxlu/3d/shapes/IcoSphere.h>


namespace roxlu {

IcoSphere::IcoSphere() {
}

void IcoSphere::create(int detail, float radius, VertexData& vertex_data) {
	
	// how we got these values:
	// http://www.bobchapman.co.uk/The_Geometry_of_the_Singapore_Ball.pdf	
    float x = 0.525731112119133606;
    float z = 0.850650808352039932;

	// default icosahedron
	vector<Vec3> tmp_verts;
	tmp_verts.push_back(Vec3( -x,  0,  z ));
    tmp_verts.push_back(Vec3(  x,  0,  z ));
    tmp_verts.push_back(Vec3( -x,  0, -z ));
    tmp_verts.push_back(Vec3(  x,  0, -z ));
    tmp_verts.push_back(Vec3(  0,  z,  x ));
    tmp_verts.push_back(Vec3(  0,  z, -x ));
    tmp_verts.push_back(Vec3(  0, -z,  x ));
    tmp_verts.push_back(Vec3(  0, -z, -z ));
    tmp_verts.push_back(Vec3(  z,  x,  0 ));
    tmp_verts.push_back(Vec3( -z,  x,  0 ));
    tmp_verts.push_back(Vec3(  z, -x,  0 ));
    tmp_verts.push_back(Vec3( -z, -x,  0 ));
	
	// triangle indices
	int idxs[] = { 
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4, 
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2, 
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};
	
	int num = 4 * 5 * 3;
	for(int i = 0; i < num; ++i) {
		vertex_data.indices.push_back(idxs[i]);
	}
	
	if(detail > 8) {
		detail = 8;
	}
	else if (detail < 0) {
		detail = 0;
	}
	
	// iterator over the number of detail level and
	for(int i = 0; i < detail; ++i) {
		vector<int> indices2;
		vector<Vec3> tmp_verts2;
		for(int j = 0, idx = 0; j < vertex_data.indices.size(); j+=3) {
			
			// add triangle indices
			indices2.push_back(idx++); 
			indices2.push_back(idx++); 
			indices2.push_back(idx++);
						
			indices2.push_back(idx++); 
			indices2.push_back(idx++); 
			indices2.push_back(idx++);
			
			indices2.push_back(idx++); 
			indices2.push_back(idx++); 
			indices2.push_back(idx++);
			
			indices2.push_back(idx++); 
			indices2.push_back(idx++); 
			indices2.push_back(idx++);
						
			// split triangle.
			Vec3 v1 = tmp_verts[vertex_data.indices[j+0]]; 
			Vec3 v2 = tmp_verts[vertex_data.indices[j+1]]; 
			Vec3 v3 = tmp_verts[vertex_data.indices[j+2]]; 
		
			v1.normalize();
			v2.normalize();
			v3.normalize();
			
			Vec3 a = (v1 + v2) * 0.5f;
			Vec3 b = (v2 + v3) * 0.5f; 
			Vec3 c = (v3 + v1) * 0.5f; 
			
			a.normalize();
			b.normalize();
			c.normalize();
			
			tmp_verts2.push_back(v1); 
			tmp_verts2.push_back(a); 
			tmp_verts2.push_back(c);
			
			tmp_verts2.push_back(a); 
			tmp_verts2.push_back(v2); 
			tmp_verts2.push_back(b);
			
			tmp_verts2.push_back(a);
			tmp_verts2.push_back(b); 
			tmp_verts2.push_back(c);
			
			tmp_verts2.push_back(c); 
			tmp_verts2.push_back(b); 
			tmp_verts2.push_back(v3);
		}
		
		tmp_verts = tmp_verts2;
		vertex_data.indices = indices2;
	}
			
	vector<Vec3>::iterator it = tmp_verts.begin();
	while(it != tmp_verts.end()) {
		vertex_data.addVertex((*it) * radius);		
		++it;
	}
	
	// add the triangles
	size_t len = vertex_data.indices.size();
	for(int i = 0; i < len; i += 3) {
		vertex_data.addTriangle(
			vertex_data.indices[i]
			,vertex_data.indices[i+1]
			,vertex_data.indices[i+2]
		);
	}
}


} // roxlu
