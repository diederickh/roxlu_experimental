#ifndef ROXLU_DELAUNAYH
#define ROXLU_DELAUNAYH

#include "Wm5Core.h"
#include "Wm5Mathematics.h"
#include "Wm5Images.h"
#include "Wm5Imagics.h"

#include <vector>
using std::vector;

namespace roxlu {

class IncrementalDelaunay2D {	

public:

	IncrementalDelaunay2D(float xmin, float ymin, float xmax, float ymax);
	~IncrementalDelaunay2D();

	void create();
	int insert(float x, float y);
	const int* getIndices();
	int getNumTriangles();
	const vector<Wm5::Vector2f>& getVertices() const;
	
	Wm5::IncrementalDelaunay2f* del;
};


inline int IncrementalDelaunay2D::getNumTriangles() {
	return del->GetNumTriangles();
}

inline const int* IncrementalDelaunay2D::getIndices() {
	return del->GetIndices();
}

inline int IncrementalDelaunay2D::insert(float x, float y) {
	return del->Insert(Wm5::Vector2f(x,y));
}

inline const vector<Wm5::Vector2f>& IncrementalDelaunay2D::getVertices() const {
	return del->GetVertices();
}

inline void IncrementalDelaunay2D::create() {
	del->GenerateRepresentation();
}

} // roxlu

#endif