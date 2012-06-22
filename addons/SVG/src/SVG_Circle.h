#ifndef ROXLU_SVG_CIRCLEH
#define ROXLU_SVG_CIRCLEH

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "SVG_Types.h"
#include "SVG_Element.h"

using std::string;
using std::vector;
using std::stringstream;


struct SVG_Circle : public SVG_Element {
	SVG_Circle(const float& cx, const float& cy, const float& r);
	string toXML();

	float cx;
	float cy;
	float r;
};


#endif