#include "HE_Face.h"
#include "HE_HalfEdge.h"
#include "HE_Vertex.h"
#include "Vec3.h"
#include "Ray.h"
#include "Utils.h"
#include <algorithm>

namespace roxlu {

HE_Face::HE_Face()
	:half_edge(NULL)
	,is_sorted(false)
{
}

HE_Face::HE_Face(HE_HalfEdge* he)
	:half_edge(he)
	,is_sorted(false)
{
}

Vec3 HE_Face::getNormal() {
	// http://www.opengl.org/wiki/Calculating_a_Surface_Normal
	HE_HalfEdge* he = half_edge;
	Vec3 normal;
	do {
		Vec3& p0 = *he->getVertex()->getPositionPtr();
		Vec3& p1 = *he->getNext()->getVertex()->getPositionPtr();
		normal.x += (p0.y - p1.y) * (p0.z + p1.z);
		normal.y += (p0.z - p1.z) * (p0.x + p1.x);
		normal.z += (p0.x - p1.x) * (p0.y + p1.y);
		he = he->getNext();
	} while(he != half_edge);
	normal.normalize();
	return normal;
}

Vec3 HE_Face::getCenter() {
	HE_HalfEdge* he = half_edge;
	int c = 0;
	Vec3 center;
	do {
		center += (*(he->getVertex()->getPositionPtr()));
		++c;
		he = he->getNext();
	} while(he != half_edge);
	return (center / c);
}

// get UNIQUE vertices from face (left most first)
const set<HE_Vertex*> HE_Face::getVertices() {
	if(!is_sorted) {
		sort();
	}
	set<HE_Vertex*> result;
	HE_HalfEdge* he = half_edge;
	do {
		result.insert(he->getVertex());
		he = he->getNext();
	} while(he != half_edge);
	
	return result;
}

// get UNIQUE edges from face (left most first)
const set<HE_Edge*> HE_Face::getEdges() {
	if(!is_sorted) {
		sort();
	}
	set<HE_Edge*> result;
	HE_HalfEdge* he = half_edge;
	do {
		result.insert(he->getEdge());
		he = he->getNext();
	} while(he != half_edge);
	return result;
}

// get UNIQUE half edges from face.
const set<HE_HalfEdge*> HE_Face::getHalfEdges() {
	if(!is_sorted) {
		sort();
	}
	set<HE_HalfEdge*> result;
	HE_HalfEdge* he = half_edge;
	do {
		result.insert(he);
		he = he->getNext();
	} while(he != half_edge);
	return result;
}


int HE_Face::getNumVertices() {
	HE_HalfEdge* he = half_edge;
	int c = 0;
	do {
		c++;
		he = he->getNext();
	} while (he != half_edge);
	return c;
}

// left most vertex is put first.
void HE_Face::sort() {
	if(half_edge != NULL) {

		HE_HalfEdge* he = half_edge;
		HE_HalfEdge* left_most = he;
		do {
			he = he->getNext();
			Vec3& vec_a = he->getVertex()->getPositionRef();
			Vec3& vec_b = left_most->getVertex()->getPositionRef();
			bool change = false;
			if(vec_a.x < vec_b.x) {
				change = true;
			}
			if(!change && vec_a.y < vec_b.y) {
				change = true;
			}
			if(!change && vec_a.z < vec_b.z) {
				change = true;
			}
			if(change) {
				left_most = he; 
			}
		} while(he != half_edge);
		half_edge = left_most;
		is_sorted = true;
	}
}

// See Essential Mathematics for Games and Interactive Applications
// Also check this article of Jacco Bikker about optimizing this: 
// http://www.devmaster.net/articles/raytracing_series/part7.php
bool HE_Face::intersectsWithRay(const Ray& r) {
	if(getNumVertices() != 3) {
		printf("HE_Face::intersectsWithRay() only implemented for triangles\n");
		return false;
	}
	
	Vec3& va = *half_edge->getVertex()->getPositionPtr();
	Vec3& vb = *(half_edge->getNext()->getVertex()->getPositionPtr());
	Vec3& vc = *(half_edge->getPrev()->getVertex()->getPositionPtr());
	Vec3 e1 = vb - va;
	Vec3 e2 = vc - va;
	Vec3 p = cross(r.direction, e2);
	float a = dot(e1, p);
	
	if(a < EPSILON) {
		return false;
	}
	
	// calc barycentric coords. (u, v)
	float inv_det = 1.0 / a;
	Vec3 s = r.origin - va;
	float u = inv_det * dot(s,p);
	if ( u <  0.0f || u > 1.0f) {
		return false;
	}
	
	Vec3 q = cross(s, e1);
	float v = inv_det * dot(r.direction,q);
	if(v < 0.0f || (u+v) > 1.0f) {
		return false;
	}

	// calc line component.
	float t = inv_det * dot(e2,q);
	return t >= 0;
}

}; // roxlu