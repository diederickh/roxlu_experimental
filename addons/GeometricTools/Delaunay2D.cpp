#include "Delaunay2D.h"

namespace roxlu {

Delaunay2D::Delaunay2D()
	:del(NULL)
{
}

void Delaunay2D::create() {
	if(del != NULL) {
		return; // already created
	}
	if(vertices.size() == 0) {
		return; // no vertices
	}
//	Wm5::Vector2f d;
	del = new Wm5::Delaunay2f(vertices.size(), (Wm5::Vector2f*)&(vertices[0].X()), 0.001f, false, Wm5::Query::QT_REAL); 
//	printf("Dim: %d\n", del->GetDimension());
	//del = new Wm5::Delaunay2f(vertices.size(), (Wm5::Vector2<Wm5::Real>*) &vertices[0].X(), 0.001f, false, Wm5::Query::QT_REAL); 
}

}