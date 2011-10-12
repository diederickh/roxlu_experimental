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
	vertex_data.vertices.assign(num_vertices, Vec3());
	vertex_data.texcoords.assign(num_vertices, Vec2());
	vertex_data.indices.assign(num_index_elements, int());
	
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
			vertex_data.vertices[dx].x = radius * sin_theta * cos_phi;
			vertex_data.vertices[dx].y = radius * sin_theta * sin_phi;
			vertex_data.vertices[dx].z = radius * cos_theta;

			vertex_data.texcoords[dx].x = phi / TWO_PI;
			vertex_data.texcoords[dx].y = theta / PI;
		}
	}
	
	// create indices.
	int dx = 0;
	for(int slice = 0; slice < slices; ++slice) {
		for(int stack = 0; stack < stacks; ++stack) {
			vertex_data.indices[dx++] = SPHERE_GET_AT(stack,slice);
			vertex_data.indices[dx++] = SPHERE_GET_AT(stack+1,slice);
			vertex_data.indices[dx++] = SPHERE_GET_AT(stack+1,slice+1);
			vertex_data.indices[dx++] = SPHERE_GET_AT(stack,slice+1);
		}
	}
}

} // roxlu