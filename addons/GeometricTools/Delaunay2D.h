#ifndef ROXLU_DELAUNAY2DH
#define ROXLU_DELAUNAY2DH

#include <vector>
using std::vector;

#include "Wm5Core.h"
#include "Wm5Mathematics.h"
#include "Wm5Images.h"
#include "Wm5Imagics.h"

namespace roxlu {






class Delaunay2D {
public:
	Delaunay2D();
	size_t insert(const float& x, const float& y);
	void clear();
	void create();
	int getNumTriangles();
	const Wm5::Vector2f* getVertices();
	const int* getIndices();
	bool getVertexSet(int i, Wm5::Vector2f result[3]) const;
	bool getIndexSet(int i, int indices[3]) const;
private:
	vector<Wm5::Vector2f> vertices;
	Wm5::Delaunay2f* del;
};

inline void Delaunay2D::clear() {
	vertices.clear();
	if(del != NULL) {
		delete del;
	}
	del = NULL;
}

inline size_t Delaunay2D::insert(const float& x, const float& y) {
	Wm5::Vector2f v(x,y);
	vertices.push_back(v);
	return vertices.size()-1;
}

// get vertices for the given triangel i
inline bool Delaunay2D::getVertexSet(int i, Wm5::Vector2f result[3]) const {
	return del->GetVertexSet(i, result);
}

// get indices for the given triangle i
inline bool Delaunay2D::getIndexSet(int i, int indices[3]) const {
	return del->GetIndexSet(i, indices);
}

inline int Delaunay2D::getNumTriangles() {
	if(del == NULL) {
		return 0;
	}
	return del->GetNumSimplices();
}

inline const Wm5::Vector2f* Delaunay2D::getVertices() {
	return del->GetVertices();
}

inline const int* Delaunay2D::getIndices() {
	return del->GetIndices();
}
}  // roxlu
#endif