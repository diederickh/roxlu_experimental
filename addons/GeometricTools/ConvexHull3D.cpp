#include "ConvexHull3D.h"

namespace roxlu {

ConvexHull3D::ConvexHull3D() 
	:vertices(NULL)
	,query_type(Wm5::Query::QT_INT64) // or Query::QT_REAL for more accuracy but less performance.
	,num_triangles(0)
	,num_indices(0)
{
}

ConvexHull3D::~ConvexHull3D() {
	clear();
}

void ConvexHull3D::clear() {
	if(vertices != NULL) {
		delete[] vertices;
	}
	positions.clear();
	num_triangles = 0;
	num_indices = 0;
}

void ConvexHull3D::create() {
	// reset
	if(vertices != NULL) {
		delete[] vertices;
	}

	num_triangles = 0; 
	num_indices = 0;
		
	// copy positions.
	int dx = 0;
	size_t num_pos = getNumPositions();
	vertices = new Wm5::Vector3f[num_pos];
	
	for(int i = 0; i < num_pos; ++i) {
		Wm5::Vector3f& v = *(vertices+i);
		dx = i * 3;
		v[0] = positions[i];
		v[1] = positions[i+1];
		v[2] = positions[i+2];
	}

	chull = new Wm5::ConvexHull3f(num_pos, vertices, 0.001f, false, query_type);
	num_triangles = chull->GetNumSimplices();
	num_indices = num_triangles * 3;
	num_vertices = num_pos;
	indices = chull->GetIndices();
}


} // roxlu