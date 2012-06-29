#ifndef ROXLU_SVG_POLYGONH
#define ROXLU_SVG_POLYGONH


#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "SVG_Types.h"
#include "SVG_Element.h"

using std::string;
using std::vector;
using std::stringstream;


struct SVG_Polygon : public SVG_Element {
	SVG_Polygon();
		
	int addPoint(const float* p);
	string toXML();

	vector<SVG_Point> points;
};



#endif