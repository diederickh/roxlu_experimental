#ifndef ROXLU_SVG_LINEH
#define ROXLU_SVG_LINEH

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "SVG_Types.h"
#include "SVG_Element.h"

using std::string;
using std::vector;
using std::stringstream;


struct SVG_Line : public SVG_Element {
	SVG_Line(const float& x0, const float& y0, const float& x1, const float& y1);
	string toXML();

	float x0, x1, y0, y1;
};


#endif