#include "SVG_Circle.h"

SVG_Circle::SVG_Circle(const float& cx, const float& cy, const float& r)
	:SVG_Element(SVG_CIRCLE)
	,cx(cx)
	,cy(cy)
	,r(r)
	
{
}

string SVG_Circle::toXML() {
	style = "stroke:" +getStrokeColorString();
	stringstream ss;
	ss << "<circle "
		<< "cx=\"" << cx << "\" " 
		<< "cy=\"" << cy << "\" "
		<< "r=\"" << r << "\" " 
		<< "style=\"stroke:" +getStrokeColorString() <<";fill:" << getFillColorString() << ";\" "	
		<< "/>";
	return ss.str();

}