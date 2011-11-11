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
	int num_vertices = (stacks + 1) * (slices + 1);
	vector<Vec3>vertices;
	vector<Vec2>texcoords;
	vertices.assign(num_vertices, Vec3());
	texcoords.assign(num_vertices, Vec2());
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
	//-----------------------------------

	/*
	int segments = slices;
	int num_vertices = (segments+1)*2;
	float *verts = new float[num_vertices*3];
	float *normals = new float[num_vertices*3];
	float *tex_coords = new float[num_vertices*2];

//	vertex_data.vertices.assign(num_vertices, Vec3());
	vertex_data.texcoords.assign(num_vertices, Vec2());
	printf("Num vertices: %d\n", num_vertices);
//	vertex_data.indices.assign(num_vertices, int());

//	glEnableClientState( GL_VERTEX_ARRAY );
//	glVertexPointer( 3, GL_FLOAT, 0, verts );
//	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
//	glEnableClientState( GL_NORMAL_ARRAY );
//	glNormalPointer( GL_FLOAT, 0, normals );
	vector<Vec3> tmp_vertices;
	vector<Vec2> tmp_texcoords;
	tmp_vertices.assign(num_vertices, Vec3());
	tmp_texcoords.assign(num_vertices, Vec2());
	for( int j = 0; j < segments / 2; j++ ) {
		float theta1 = j * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );
		float theta2 = (j + 1) * 2 * 3.14159f / segments - ( 3.14159f / 2.0f );

		for( int i = 0; i <= segments; i++ ) {
			Vec3 e, p;
			float theta3 = i * 2 * 3.14159f / segments;

			e.x = cos( theta1 ) * cos( theta3 );
			e.y = sin( theta1 );
			e.z = cos( theta1 ) * sin( theta3 );
			p = e * radius;
			normals[i*3*2+0] = e.x; 
			normals[i*3*2+1] = e.y; 
			normals[i*3*2+2] = e.z;
			Vec2 tc(0.999f - i / (float)segments, 0.999f - 2 * j / (float)segments);
			if(tc.x < 0) {
				//tc.x = 0;
			}
			if(tc.y < 0){ 
				//tc.y = 0;
			}
			
			
		//	vertex_data.addTexCoord(tc);
			tex_coords[i*2*2+0] = 0.999f - i / (float)segments; 
			tex_coords[i*2*2+1] = 0.999f - 2 * j / (float)segments;
			verts[i*3*2+0] = p.x; 
			verts[i*3*2+1] = p.y; 
			verts[i*3*2+2] = p.z;
//			vertex_data.addVertex(p);
			//tmp_vertices.push_back(p);
			//tmp_texcoords.push_back(tc);
			tmp_vertices[DX_AT(i,j)] = p;
			tmp_texcoords[DX_AT(i,j)] = tc;
			
			if(tc.x > 1 || tc.y > 1) {
				printf("%f, %f\n", tc.x, tc.y);
			}
			if(tc.x < 0 || tc.y < 0) {
				printf("%f, %f\n", tc.x, tc.y);
			}
			
		
			e.x = cos( theta2 ) * cos( theta3 );
			e.y = sin( theta2 );
			e.z = cos( theta2 ) * sin( theta3 );
			p = e * radius;
			normals[i*3*2+3] = e.x; 
			normals[i*3*2+4] = e.y; 
			normals[i*3*2+5] = e.z;
			tex_coords[i*2*2+2] = 0.999f - i / (float)segments; 
			tex_coords[i*2*2+3] = 0.999f - 2 * ( j + 1 ) / (float)segments;
			tc.set(0.999f - i / (float)segments,  0.999f - 2 * ( j + 1 ) / (float)segments);
			
			if(tc.x < 0) {
				//tc.x = 0;
			}
			if(tc.y < 0){ 
				//tc.y = 0;
			}
			
			if(tc.x > 1 || tc.y > 1) {
				printf("%f, %f\n", tc.x, tc.y);
			}
			if(tc.x < 0 || tc.y < 0) {
				printf("%f, %f\n", tc.x, tc.y);
			}
//			vertex_data.addTexCoord(tc);
			verts[i*3*2+3] = p.x; 
			verts[i*3*2+4] = p.y; 
			verts[i*3*2+5] = p.z;
			//vertex_data.vertices[(i+1)*3*2] = p;
//			vertex_data.addVertex(p);
//			tmp_vertices.push_back(p);
//			tmp_texcoords.push_back(tc);
			tmp_vertices[DX_AT(i,j+1)] = p;
			tmp_texcoords[DX_AT(i,j+1)] = tc;

		}
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, (segments + 1)*2 );
	}
	
	// add quads
	//int n = vertex_data.getNumVertices();
//	int n = tmp_vertices.size();
//	for(int i = 0; i < n-2; i+=2) {	
//		//vertex_data.addQuad(i, i+1, i+3, i+2);
//		Vec3 a = tmp_vertices.at(i);
//		Vec3 b = tmp_vertices.at(i+1);
//		Vec3 c = tmp_vertices.at(i+3);
//		Vec3 d = tmp_vertices.at(i+2);
//		vertex_data.addTexCoord(tmp_texcoords[i]);
//		vertex_data.addTexCoord(tmp_texcoords[i+1]);
//		vertex_data.addTexCoord(tmp_texcoords[i+3]);
//		vertex_data.addTexCoord(tmp_texcoords[i+2]);
//		int da = vertex_data.addVertex(a);
//		int db = vertex_data.addVertex(b);
//		int dc = vertex_data.addVertex(c);
//		int dd = vertex_data.addVertex(d);
//		vertex_data.addQuad(da, db, dc, dd);
//	}

	for(int j = 0; j < segments/2; ++j) {
		for(int i = 0; i <= segments; ++i) {
			//int dx = (j * segments) + i;
			int a = DX_AT(i,j+1);
			
			int b;
			int c; 
			if(i+1 >= segments) {
				b = DX_AT(i,j+1);
				c = DX_AT(i, j);
			}
			else {
				b = DX_AT(i+1,j+1);
				c = DX_AT(i+1, j);
			}
			
			int d = DX_AT(i,j);
			vertex_data.addVertex(tmp_vertices[a]);
			vertex_data.addVertex(tmp_vertices[b]);
			vertex_data.addVertex(tmp_vertices[c]);
			vertex_data.addVertex(tmp_vertices[d]);
			vertex_data.addQuad(a,b,c,d);
		}
	}

	printf("Created vertices: %d\n", vertex_data.getNumVertices());
	*/
	
	/*

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
			
			printf("Created vertex: %d\n", dx);

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
//			int dxa = SPHERE_GET_AT(stack,slice);
//			int dxb = SPHERE_GET_AT(stack+1,slice);
//			int dxc = SPHERE_GET_AT(stack+1,slice+1);
//			int dxd = SPHERE_GET_AT(stack,slice+1);

			int dxa = SPHERE_GET_AT(stack+1,slice);
			int dxb = SPHERE_GET_AT(stack+1,slice+1);
			int dxc = SPHERE_GET_AT(stack,slice+1);
			int dxd = SPHERE_GET_AT(stack,slice);
//					
			Vec3 va = vertices[dxa];
			Vec3 vb = vertices[dxb];
			Vec3 vc = vertices[dxc];
			Vec3 vd = vertices[dxd];
			
			vertex_data.vertices.push_back(va);
			vertex_data.vertices.push_back(vb);
			vertex_data.vertices.push_back(vc);
			vertex_data.vertices.push_back(vd);
			
			// created for export... not working correctly for the ply format
			printf("%d, %d, %d, %d\n", dxa, dxb, dxc, dxd);
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
	ready:
	vertex_data.enablePositionAttrib();
	vertex_data.enableTexCoordAttrib();
//	vertex_data.enableNormalAttrib();
*/
}

} // roxlu