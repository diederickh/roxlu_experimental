#include "SVG_Polygon.h"

SVG_Polygon::SVG_Polygon() 
	:SVG_Element(SVG_POLYGON)
{
}

	
int SVG_Polygon::addPoint(const float* p) {
	SVG_Point point(p);
	points.push_back(p);
	return points.size()-1;
}

string SVG_Polygon::toXML() {
	stringstream ss;
	ss << "<polygon points=\"";
	for(vector<SVG_Point>::iterator it = points.begin(); it != points.end(); ++it) {
		SVG_Point& p = *it;
		ss << p.x << "," << p.y << " ";
	}
	ss 	<< "\" "
		<< "style=\"stroke:" +getStrokeColorString() <<";fill:" << getFillColorString() << ";\" "
		<< "/>";
	return ss.str();
}

