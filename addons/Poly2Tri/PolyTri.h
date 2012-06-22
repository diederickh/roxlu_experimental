#ifndef ROLXU_POLYTRIH
#define ROLXU_POLYTRIH

#include "poly2tri.h"
#include <vector>
#include <list>
#include "OpenGL.h"


using std::vector;

class PolyTri {
public:
	PolyTri();
	void setup(const float& minX, const float& minY, const float& maxX, const float& maxY);
	void debugDraw();
	void drawMap();
	void addHole(const vector<p2t::Point*>& points);
	void addHole(const float* p, const size_t num);
	void addPoints(const float* p, const size_t num, const size_t stride);
	void addPoints(const float* p, const size_t num);
	void addPoint(const float& x, const float& y);
	void triangulate();
	
	std::list<p2t::Triangle*>& getMap();
	vector<p2t::Triangle*>& getTriangles();
	
	bool is_triangulated;
	p2t::CDT* cdt;

	std::list<p2t::Triangle*> map;
	vector<p2t::Triangle*> triangles;
//	float min_x, min_y;
//	float max_x, max_y;
};

#endif
