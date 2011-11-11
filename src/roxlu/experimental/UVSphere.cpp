#include "Constants.h"
#include "UVSphere.h"
#include "VertexData.h"
#include "Vec3.h"


#define SPHERE_GET_AT(stack,slice) (int)((stack) * ((slices)+1)) + (slice)

namespace roxlu {

UVSphere::UVSphere() {
}

UVSphere::~UVSphere() {
}

void UVSphere::create(float radius, int slices, int stacks, VertexData& vertex_data) {
	// calculate number of quads and indices.
	int num_vertices = (stacks + 1) * (slices + 1);
	int num_index_elements = (stacks) * (slices) * 4;
//	vertex_data.vertices.assign(num_index_elements, Vec3());
//	vertex_data.texcoords.assign(num_index_elements, Vec2());
//	vertex_data.indices.assign(num_index_elements, int());
	
	vector<Vec3> vertices;
//	vector<Vec3> normals;
	vector<Vec2> texcoords;
	vertices.assign(num_index_elements, Vec3());
//	normals.assign(num_index_elements, Vec3());
	texcoords.assign(num_index_elements, Vec2());
	
	// create sphere coordinates + texture coords.
	for(float stack = 0; stack <= stacks; ++stack) {
		float theta = stack/(float)stacks*PI;
		float sin_theta = sinf(theta);		
		float cos_theta = cosf(theta);
			
		for(float slice = 0; slice <= slices; ++slice) {
			float phi = slice /(float) (slices) * TWO_PI;
			float sin_phi = sinf(phi);
			float cos_phi = cosf(phi);
		
			int dx = SPHERE_GET_AT(stack, slice);
			
			// when you want to use indices (but give problems with per face
			// normals.
			//vertex_data.vertices[dx].x = radius * sin_theta * cos_phi;
			//vertex_data.vertices[dx].y = radius * sin_theta * sin_phi;
			//vertex_data.vertices[dx].z = radius * cos_theta;
			//vertex_data.texcoords[dx].x = phi / TWO_PI;
			//vertex_data.texcoords[dx].y = theta / PI;

			vertices[dx].x = radius * sin_theta * cos_phi;
			vertices[dx].y = radius * sin_theta * sin_phi;
			vertices[dx].z = radius * cos_theta;
			

			texcoords[dx].x = phi / TWO_PI;
			texcoords[dx].y = theta / PI;
		}
	}
	// create indices / vertices; commented code creates shared vertices.
	int dx = 0;
	for(int slice = 0; slice < slices; ++slice) {
		for(int stack = 0; stack < stacks; ++stack) {
			// when you want to use an indexed sphere (but this gives problems 
			// when you want a per face normal.
			//vertex_data.indices[dx++] = SPHERE_GET_AT(stack+1,slice);
			//vertex_data.indices[dx++] = SPHERE_GET_AT(stack+1,slice+1);
			//vertex_data.indices[dx++] = SPHERE_GET_AT(stack,slice+1);
			//vertex_data.indices[dx++] = SPHERE_GET_AT(stack,slice);
			
//			int dxa = SPHERE_GET_AT(stack+1,slice);
//			int dxb = SPHERE_GET_AT(stack+1,slice+1);
//			int dxc = SPHERE_GET_AT(stack,slice+1);
//			int dxd = SPHERE_GET_AT(stack,slice);
			int dxa = SPHERE_GET_AT(stack,slice);
			int dxb = SPHERE_GET_AT(stack+1,slice);
			int dxc = SPHERE_GET_AT(stack+1,slice+1);
			int dxd = SPHERE_GET_AT(stack,slice+1);
	
			
			Vec3 va = vertices[dxa];
			Vec3 vb = vertices[dxb];
			Vec3 vc = vertices[dxc];
			Vec3 vd = vertices[dxd];
			
			vertex_data.vertices.push_back(va);
			vertex_data.vertices.push_back(vb);
			vertex_data.vertices.push_back(vc);
			vertex_data.vertices.push_back(vd);
			
			// created for export... not working correctly for the ply format
			vertex_data.addQuad(dxa, dxb, dxc, dxd);
			
			vertex_data.texcoords.push_back(texcoords[dxa]);
			vertex_data.texcoords.push_back(texcoords[dxb]);
			vertex_data.texcoords.push_back(texcoords[dxc]);
			vertex_data.texcoords.push_back(texcoords[dxd]);
			
			// normal.
			Vec3 ab = vb-va;
			Vec3 bc = vb-vc;
			Vec3 normal = bc.cross(ab).normalize();
			vertex_data.normals.push_back(normal);
			vertex_data.normals.push_back(normal);
			vertex_data.normals.push_back(normal);
			vertex_data.normals.push_back(normal);
			
			
//			vertex_data.addTriangle(dxa, dxb, dxc);
//			vertex_data.addTriangle(dxc, dxd, dxa);
		}
	}
	
	vertex_data.enablePositionAttrib();
	vertex_data.enableTexCoordAttrib();
//	vertex_data.enableNormalAttrib();
}

} // roxlu