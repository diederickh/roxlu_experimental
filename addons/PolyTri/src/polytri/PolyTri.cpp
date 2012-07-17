#include <polytri/PolyTri.h>

// TODO: cleanup when destroyed! it's leaking now

PolyTri::PolyTri() 
	:cdt(NULL)
	,is_triangulated(false)
{

}

// i.e. -ofGetWidth(), -ofGetHeight(), ofGetWidth(), ofGetHeight()
void PolyTri::setup(const float& minX, const float& minY, const float& maxX, const float& maxY) {
	
	if(cdt != NULL) {
		delete cdt;
	}
	
	
	vector<p2t::Point*> bounding;
	bounding.push_back(new p2t::Point(minX,minY));
	bounding.push_back(new p2t::Point(maxX,minY));
	bounding.push_back(new p2t::Point(maxX,maxX));
	bounding.push_back(new p2t::Point(minX,maxY));
	
	cdt = new p2t::CDT(bounding);


}

std::list<p2t::Triangle*>& PolyTri::getMap() {
	return map;
}

void PolyTri::triangulate() {
	if(cdt == NULL) {
		return;
	}

	cdt->Triangulate();
	triangles = cdt->GetTriangles();
	map = cdt->GetMap();
	is_triangulated = true;
}

void PolyTri::addHole(const float* p, const size_t num) {
	vector<p2t::Point*> hole;
	for(int i = 0; i < num; ++i){ 
		int dx = i * 2;
		hole.push_back(new p2t::Point(*(p+dx), *(p+dx+1)));
	}
	cdt->AddHole(hole);
}

void PolyTri::addHole(const vector<p2t::Point*>& points) {
	cdt->AddHole(points);
}


void PolyTri::addPoints(const float* p, const size_t num, const size_t stride) {
	int dx;
	for(int i = 0; i < num; ++i) {
		dx = i * stride;
		addPoint(*(p+dx), *(p+dx+1));
	}
}

void PolyTri::addPoints(const float* p, const size_t num) {
	int dx;
	for(int i = 0; i < num; ++i) {
		dx = i * 2;
		addPoint(*(p+dx), *(p+dx+1));
	}
}

void PolyTri::addPoint(const float& x, const float& y) {
	cdt->AddPoint(new p2t::Point(x,y));
}

void PolyTri::debugDraw() {
	if(!is_triangulated) {
		return;
	}

	glColor3f(0,0,0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	for(vector<p2t::Triangle*>::iterator it = triangles.begin(); it != triangles.end(); ++it) {
		p2t::Triangle& tri = *(*it);
		p2t::Point& a = *tri.GetPoint(0); glVertex2f(a.x, a.y);
		p2t::Point& b = *tri.GetPoint(1); glVertex2f(b.x, b.y);
		p2t::Point& c = *tri.GetPoint(2); glVertex2f(c.x, c.y);
		
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	
}

void PolyTri::drawMap() {
	glColor3f(0,1,0);
	
	for(std::list<p2t::Triangle*>::iterator it = map.begin(); it != map.end(); ++it) {
		p2t::Triangle& tri = *(*it);
		p2t::Point& a = *tri.GetPoint(0); 
		p2t::Point& b = *tri.GetPoint(1); 
		p2t::Point& c = *tri.GetPoint(2); 
		
		/*
		 ConstrainedColor(t.constrained_edge[2]);
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd( );

    ConstrainedColor(t.constrained_edge[0]);
    glBegin(GL_LINES);
    glVertex2f(b.x, b.y);
    glVertex2f(c.x, c.y);
    glEnd( );

    ConstrainedColor(t.constrained_edge[1]);
    glBegin(GL_LINES);
    glVertex2f(c.x, c.y);
    glVertex2f(a.x, a.y);
    glEnd( );
		*/

		if(tri.constrained_edge[2]) { glColor3f(1.0,0,0); } else { glColor3f(0,1,0); }
		glBegin(GL_LINES);
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
		glEnd();		
		
		if(tri.constrained_edge[0]) { glColor3f(1.0,0,0); } else { glColor3f(0,1,0); }
		glBegin(GL_LINES);
			glVertex2f(b.x, b.y);
			glVertex2f(c.x, c.y);
		glEnd();			
		
		if(tri.constrained_edge[1]) { glColor3f(1.0,0,0); } else { glColor3f(0,1,0); }
		glBegin(GL_LINES);
			glVertex2f(c.x, c.y);
			glVertex2f(a.x, a.y);
		glEnd();		
	}
	glEnd();
}


