#ifndef ROXLU_SVG
#define ROXLU_SVG

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "SVG_Types.h"
#include "SVG_Element.h"
#include "SVG_Polygon.h"
#include "SVG_Line.h"
#include "SVG_Circle.h"

using std::string;
using std::vector;
using std::stringstream;
using std::ofstream;


class SVG {
public:
	SVG_Line* addLine(const float* p0, const float* p1);
	SVG_Line* addLine(const float& x0, const float& y0, const float& x1, const float& y1);
	SVG_Polygon* addPolygon();
	SVG_Polygon* addTriangle(const float* a, const float* b, const float* c);
	SVG_Circle* addCircle(const float& cx, const float& cy, const float& radius);
	bool save(const string& filePath);
	vector<SVG_Element*> elements;
};

#endif