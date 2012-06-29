#include "SVG_Line.h"


SVG_Line::SVG_Line(const float& x0, const float& y0, const float& x1, const float& y1)
	:SVG_Element(SVG_LINE)
	,x0(x0)
	,y0(y0)
	,x1(x1)
	,y1(y1)
{
	
}
	

string SVG_Line::toXML() {
	style = "stroke:" +getStrokeColorString();
	stringstream ss;
	ss << "<line "
		<< "x1=\"" << x0 << "\" " 
		<< "y1=\"" << y0 << "\" "
		<< "x2=\"" << x1 << "\" " 
		<< "y2=\"" << y1 << "\" "
		<< "style=\"stroke-width:0.1;" << style << ";\" "
		<< "/>";
	return ss.str();
}
