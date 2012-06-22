#include "SVG.h"

SVG_Line* SVG::addLine(const float* p0, const float* p1) {
	return addLine(p0[0], p0[1], p1[0], p1[1]);
}

SVG_Line* SVG::addLine(const float& x0, const float& y0, const float& x1, const float& y1) {
	SVG_Line* line = new SVG_Line(x0, y0, x1, y1);
	elements.push_back(line);
	return line;
}

SVG_Polygon* SVG::addTriangle(const float* a, const float* b, const float* c) {
	SVG_Polygon* p = addPolygon();
	p->addPoint(a);
	p->addPoint(b);
	p->addPoint(c);
	return p;
}

SVG_Polygon* SVG::addPolygon() {
	SVG_Polygon* poly = new SVG_Polygon();
	elements.push_back(poly);
	return poly;
}

SVG_Circle* SVG::addCircle(const float& cx, const float& cy, const float& radius) {
	SVG_Circle* c = new SVG_Circle(cx, cy, radius);
	elements.push_back(c);
	return c;
}

bool SVG::save(const string& filePath) {
	ofstream ofs;
	ofs.open(filePath.c_str());
	if(!ofs.is_open()) {
		printf("Cannot open: %s\n", filePath.c_str());
		return false;
	}
	
	ofs << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << "\n";
	
	for(vector<SVG_Element*>::iterator it = elements.begin(); it != elements.end(); ++it) {
		ofs << "\t" << (*it)->toXML() << "\n";
	}
	ofs << "</svg>";
	ofs.close();
	return true;
}
	