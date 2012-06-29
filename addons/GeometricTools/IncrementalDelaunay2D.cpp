#include "IncrementalDelaunay2D.h"

namespace roxlu {

IncrementalDelaunay2D::IncrementalDelaunay2D(float xmin, float ymin, float xmax, float ymax) {
	del = new Wm5::IncrementalDelaunay2f(xmin, ymin, xmax, ymax);
}

IncrementalDelaunay2D::~IncrementalDelaunay2D() {
	delete del;
}

} // roxlu