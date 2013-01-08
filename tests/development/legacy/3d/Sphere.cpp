//#include "Sphere.h"
//#include "Constants.h"

#include <roxlu/core/Constants.h>
#include <roxlu/3d/shapes/Sphere.h>


/*
	WILL BE REPLACED BY UVSPHERE
*/
namespace roxlu {

Sphere::Sphere(float nRadius, float nStacks, float nSlices) 
:radius(nRadius)
,stacks(nStacks)
,slices(nSlices)
{
	num_vertices = (nStacks + 1) * (nSlices + 1);
	num_index_elements = (nStacks) * (nSlices) * 4;
	vertices = new VertexPTNC[num_vertices];
	index_elements = new int[num_index_elements];
	
	createVertices();
	createIndices();
};

Sphere::~Sphere() {
	delete[] vertices;
	delete[] index_elements;
}

int Sphere::getAt(int nSliceItem, int nStackItem) {
	return (nStackItem * (slices+1)) + nSliceItem;
}

// generate spherical coordinates.
void Sphere::createVertices() {
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
			vertices[index].pos.set(x * radius*1.05,y * radius, z * radius);
			vertices[index].norm.set(x,y,z);
			vertices[index].col.set(x,y,z);
			vertices[index].tex.set(u,v);
		}
	}
	
	// now all vertices have been created we can add the tangent and binormal.
	for(int i = 0; i <= stacks; ++i) {
		for(int j = 0; j <= slices; ++j) {
			// last one.
			Vec3 va = vertices[getAt(i,j)].pos;
			Vec3 vb;
			if(j == slices) {
				vb = vertices[getAt(i-1, j)].pos;
			}
			else {
				vb = vertices[getAt(i + 1, j)].pos;		
			}
			// TODO add data structure with tangent
			//Vec3 dir = vb - va;
			//dir.normalize();
			//vertices[getAt(i,j)].tangent = dir;
		}
	}
};

void Sphere::createIndices() {
	// create index elements for quads.
	int dx = 0;
	for(int i = 0; i < slices; ++i) {
		for(int j = 0; j < stacks; ++j) {
			index_elements[dx++] = getAt(i,j);
			index_elements[dx++] = getAt(i+1,j);
			index_elements[dx++] = getAt(i+1,j+1);
			index_elements[dx++] = getAt(i,j+1);
		}
	}
}

void Sphere::draw() {
	//createVertices();
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	for(int i = 0; i < num_index_elements; ++i) {
		setVertexForIndex(i);
	}
	glEnd();
	return;
	glPushMatrix();
	glScalef(1.1,1.1,1.1);
	glColor3f(1.0,0.8,0.2);
	glPointSize(4);
	glBegin(GL_POINTS);
	for(int i = 0; i < num_vertices; ++i) {
		glVertex3f(
			 vertices[i].pos.x
			,vertices[i].pos.y
			,vertices[i].pos.z
		);
	}
	glEnd();
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
}

void Sphere::setVertexForIndex(int nIndex) {
	int dx = index_elements[nIndex];
	VertexPTNC v = vertices[dx];
	glTexCoord2f(v.tex.x, v.tex.y);
	glVertex3f(v.pos.x, v.pos.y, v.pos.z);
}

int Sphere::getNumVertices() {
	return num_vertices;
}

VertexPTNC* Sphere::getVertices() {
	return vertices;
}

int Sphere::getNumIndices() {
	return num_index_elements;
}
int* Sphere::getIndices() {
	return index_elements;
}

}
