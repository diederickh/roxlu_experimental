#ifndef ROXLU_SVG_ELEMENTH
#define ROXLU_SVG_ELEMENTH

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "SVG_Types.h"

using std::string;
using std::vector;
using std::stringstream;

struct SVG_Element {
	SVG_Element(SVG_Type type)
		:type(type)
	{
		stroke_color[0] = 0;
		stroke_color[1] = 0;
		stroke_color[2] = 0;
		stroke_color[3] = 0;
		
		fill_color[0] = 1.0;
		fill_color[1] = 1.0;
		fill_color[2] = 1.0;
		fill_color[3] = 1.0;
	}
	
	void setStyle(const string& st) {
		style = st;
	}
	
	void setStrokeColor(const float& r, const float& g, const float& b, float a = 1.0) {
		stroke_color[0] = r;
		stroke_color[1] = g; 
		stroke_color[2] = b;
		stroke_color[3] = a;
	}

	void setFillColor(const float& r, const float& g, const float& b, float a = 1.0) {
		fill_color[0] = r;
		fill_color[1] = g; 
		fill_color[2] = b;
		fill_color[3] = a;
	}
	
	string getStrokeColorString() {
		return getColorString(stroke_color);
	}
	
	string getFillColorString() {
		return getColorString(fill_color);
	}

	string getColorString(const float* col) {
		//printf("%f,%f,%f\n", col[0], col[1], col[2]);
		char rgb[50];
		sprintf(rgb, "%02X%02X%02X", int(col[0]*255), int(col[1]*255), int(col[2]*255));
		stringstream ss;
		ss << "#" << rgb;
		return ss.str();
	}

	virtual string toXML() = 0;
	
	float fill_color[4];
	float stroke_color[4];
	string style;

	int type;
};


#endif