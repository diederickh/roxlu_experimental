#include "Constants.h"
#include "UVSphere.h"
#include "VertexData.h"
#include "Vec3.h"


#define SPHERE_GET_AT(stack,slice) (int)((stack) * ((slices)+1)) + (slice)
#define DX_AT(i,j) (int)((j)*(segments)) + (i)

namespace roxlu {

UVSphere::UVSphere() {
}

UVSphere::~UVSphere() {
}

int UVSphere::getAt(int nSliceItem, int nStackItem) {
	return (nStackItem * (slices+1)) + nSliceItem;
}

void UVSphere::create(float radius, int numSlices, int numStacks, VertexData& vertex_data) {
	slices = numSlices;
	stacks = numStacks;
	
	// this code works; though as with all 4 other tested methods I get a wierd
	// lines when using a texture. See git history for old code.
	for (int j = 0; j < stacks; j++) {
         double latitude1 = (PI/stacks) * j - PI/2;
         double latitude2 = (PI/stacks) * (j+1) -PI/2;
         double sin1 = sin(latitude1);
         double cos1 = cos(latitude1);
         double sin2 = sin(latitude2);
         double cos2 = cos(latitude2);

         for (int i = 0; i <= slices; i++) {
            double longitude = (2*PI/slices) * i;
            double sin_long = sin(longitude);
            double cos_long = cos(longitude);
            double x1 = cos_long * cos1;
            double y1 = sin_long * cos1;
            double z1 = sin1;
            double x2 = cos_long * cos2;
            double y2 = sin_long * cos2;
            double z2 = sin2;
			
			vertex_data.addVertex(Vec3(x1 * radius, y1*radius, z1 * radius));
			vertex_data.addTexCoord(1-1.0/slices * i, 1.0/stacks * j);
			vertex_data.addNormal(Vec3(x1, y1, z1));
			
			vertex_data.addVertex(Vec3(x2 * radius, y2*radius, z2 * radius));
			vertex_data.addTexCoord(1-1.0/slices * i, 1.0/stacks * (j+1));
			vertex_data.addNormal(Vec3(x2, y2, z2));

		}
	}
	int n = vertex_data.getNumVertices();
	for(int i = 0; i < n-2; i+=2) {
		vertex_data.addQuad(i+1, i+3, i+2, i);
	}
		
	/*
	int nv = stacks;
	int nu = slices;
	
	vector<Vec3> vertices;
	vector<Vec2> texcoords;
	
	vertices.push_back(Vec3(0,0,radius));
	vertices.push_back(Vec3(0,0,-radius));

	for(int i = 0; i < stacks; ++i) {
		float t = i / (float) stacks;
		float sin_v = sin(TWO_PI * t);
		float cos_v = cos(TWO_PI * t);
		
		for(int j = 1; j < slices; ++j) {
			float s = j / (float) slices;
			float sin_u = sin(PI * s);
			float cos_u = cos(PI * s);
			vertices.push_back(Vec3(
				cos_v * sin_u * radius
				,sin_v * sin_u * radius
				,cos_u * radius
			));
			texcoords.push_back(Vec2(s,t));
		}
	}
	vector<int> indices;
	for(int k = 0; k < nv; ++k) {
		indices.push_back(0);
		indices.push_back(2 + k * (nu - 1));
		indices.push_back(2 + ((k + 1) % nv) * (nu - 1));
		
		indices.push_back(1);
		indices.push_back(((k + 1) % nv) * (nu - 1) + nu);
		indices.push_back(k * (nu - 1) + nu); 
		
		for(int l = 0; l < (nu-2); ++l) {
			indices.push_back(2 + k * (nu - 1) + l);
			indices.push_back(2 + k * (nu - 1) + l + 1);
			indices.push_back( 2 + ((k + 1) % nv) * (nu - 1) + l);
			
			indices.push_back(2 + k * (nu - 1) + l + 1);
			indices.push_back(2 + ((k + 1) % nv) * (nu - 1) + l + 1);
			indices.push_back(2 + ((k + 1) % nv) * (nu - 1) + l);
		}
	}
	for(int i = 0; i < indices.size(); ++i) {
		vertex_data.addVertex(vertices[indices[i]]);
		vertex_data.addTexCoord(texcoords[indices[i]]);
	}
	
	printf("Num indices: %d\n", indices.size());
	*/
	
	
	/*
	int num_vertices = (stacks + 1) * (slices + 1);
	vector<Vec3>vertices;
	vector<Vec2>texcoords;
	vertices.assign(num_vertices, Vec3());
	texcoords.assign(num_vertices, Vec2());
	
	// calculate vertex + texcoords
	for(float stack = 0; stack <= stacks; ++stack) {
		float theta = stack/(stacks) * PI;
		float sin_theta = sinf(theta);
		float cos_theta = cosf(theta);
			
		for(float slice = 0; slice <= slices; ++slice) {
			float phi = slice/(slices) * TWO_PI;
			float sin_phi = sinf(phi);
			float cos_phi = cosf(phi);

			float x = sin_theta * cos_phi;
			float y = sin_theta * sin_phi;
			float z = cos_theta;
			
			float u = (sin_theta * cos_phi * 0.5) + 0.5;
			float v = (sin_theta * sin_phi * 0.5) + 0.5;
		
			int index = (int)(slice + (stack * (slices+1)));
			vertices[index].set(x * radius,y * radius, z * radius);
			texcoords[index].set(u,v);
		}
	}
	
	// Create the quads
	int dx = 0;
	int num_index_elements = (stacks) * (slices) * 4;
	num_index_elements = (stacks) * (slices) * 4;	
	int* index_elements = new int[num_index_elements];

	for(int i = 0; i < slices; ++i) {
		for(int j = 0; j < stacks; ++j) {
			index_elements[dx++] = getAt(i,j);
			index_elements[dx++] = getAt(i+1,j);
			index_elements[dx++] = getAt(i+1,j+1);
			index_elements[dx++] = getAt(i,j+1);
		}
	}
	
	for(int i = 0; i < num_index_elements;++i) {
		Vec3 v = vertices[index_elements[i]];
		vertex_data.addVertex(v);
		vertex_data.addTexCoord(texcoords[index_elements[i]]);
	}
	*/
}

} // roxlu