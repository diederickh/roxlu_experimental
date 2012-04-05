#include "ConvexHull3D.h"
#include <fstream>
#include <sstream>

namespace roxlu {

ConvexHull3D::ConvexHull3D() 
	:vertices(NULL)
	,chull(NULL)
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
	if(chull != NULL) {
		delete chull;
	}
	positions.clear();
	num_triangles = 0;
	num_indices = 0;
	chull = NULL;
	vertices = NULL;
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
		v[0] = positions[dx];
		v[1] = positions[dx+1];
		v[2] = positions[dx+2];
	}

	chull = new Wm5::ConvexHull3f(num_pos, vertices, 0.001f, false, query_type);
	num_triangles = chull->GetNumSimplices();
	num_indices = num_triangles * 3;
	num_vertices = num_pos;
	indices = chull->GetIndices();
}

bool ConvexHull3D::save(const string& filepath) {
	std::ofstream ofs(filepath.c_str());
	if(!ofs.is_open()) {
		return false;
	}
	ofs << toString();
	ofs.close();
	return true;
}

string ConvexHull3D::toString() {
	int dx = 0;
	std::stringstream ss;
	size_t num_pos = getNumPositions();
	ss << num_pos << std::endl;
	for(int i = 0; i < num_pos; ++i) {
		dx = i * 3;
		ss << positions[dx] 
			<< " " << positions[dx+1]
			<< " " << positions[dx+2]
			<< std::endl;
		
	}
	return ss.str();
}

bool ConvexHull3D::load(const string& filepath) {
	clear();
	std::ifstream ifs(filepath.c_str());
	if(!ifs.is_open()) {
		return false;
	}
	int num;
	ifs >> num;
	int skip;
	ifs >> skip;
	while(ifs) {
		float x,y,z;
		ifs >> x;
		ifs >> y;
		ifs >> z;
		ifs >> skip; // endl
		addPosition(x,y,z);
	}
	create();
	return true;
}

} // roxlu