#include "HE_Debug.h"
#include "HE_Mesh.h"
#include "ofMain.h"
//#include "simplexnoise1234.h"

void hsv_to_rgb(float *rgb, float h, float s, float v) {
 	double f, w, q, t, d;

    d = h * 5.99999999;
    f = d - floor (d);
    w = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    if (d < 1.0) {
        *rgb++ = v;
        *rgb++ = t;
        *rgb++ = w;
    } else if (d < 2.0) {
        *rgb++ = q;
        *rgb++ = v;
        *rgb++ = w;
    } else if (d < 3.0) {
        *rgb++ = w;
        *rgb++ = v;
        *rgb++ = t;
    } else if (d < 4.0) {
        *rgb++ = w;
        *rgb++ = q;
        *rgb++ = v;
    } else if (d < 5.0) {
        *rgb++ = t;
        *rgb++ = w;
        *rgb++ = v;
    } else {
        *rgb++ = v;
        *rgb++ = w;
        *rgb++ = q;
    }
}

namespace roxlu {

HE_Debug::HE_Debug() {
}


void HE_Debug::drawPaired(HE_Mesh& m) {
	glColor3f(0.255,0.803,0.27);
	glBegin(GL_LINES);
		vector<HE_HalfEdge*> edges = m.getHalfEdges();
		vector<HE_HalfEdge*>::iterator it = edges.begin();
		while(it != edges.end()) {
			HE_HalfEdge& he = *(*it);
			if(he.getPair() != NULL) {
				glVertex3fv(&he.getVertex()->getPositionPtr()->x);
				glVertex3fv(&he.getPair()->getVertex()->getPositionPtr()->x);
			}
			++it;
		}
	glEnd();
	glColor3f(1,1,1);
}

void HE_Debug::drawUnpaired(HE_Mesh& m) {
	glColor3f(1.0, 0.17, 0.22);
	glBegin(GL_LINES);
		vector<HE_HalfEdge*> edges = m.getUnpairedHalfEdges();
		vector<HE_HalfEdge*>::iterator it = edges.begin();
		while(it != edges.end()) {
			HE_HalfEdge& he = *(*it);
			glVertex3fv(&he.getVertex()->getPositionPtr()->x);
			glVertex3fv(&he.getNext()->getVertex()->getPositionPtr()->x);
			++it;
		}
	glEnd();
	glColor3f(1,1,1);
}

void HE_Debug::drawFaceNormals(HE_Mesh& m, float len) {
	glColor3f(0, 0.63, 1.0);
	glBegin(GL_LINES);
		const vector<HE_Face*>& faces = m.getFacesRef();
		vector<HE_Face*>::const_iterator face_it = faces.begin();
		while(face_it != faces.end()) {
			HE_Face& f = *(*face_it);
			Vec3 center = f.getCenter();
			Vec3 normal = f.getNormal();
			normal.scale(len);
			Vec3 end = center + normal;
			glVertex3fv(ptr3(center));
			glVertex3fv(ptr3(end));
			++face_it;
		}
		
	glEnd();
	glColor3f(1,1,1);
}

void HE_Debug::drawFacesWithRandomColors(HE_Mesh& m) {
	float col[3];
	//hsv_to_rgb(col, 0, 0.4, 1.0);
	//cout << col[0] <<  ", " << col[1] << ", " << col[2] << "<---------" << endl;
	//static float kuler_cols[] = {0.5725,0,0.3608,0,0,0.0863,0,0,0,0,0.6902,0.0078,0,0.0235,0,0.4941,0.3059,0.0235,0,0.0314,0.4784,0,0.0549,0.0392,0,0,0.0627,0.7137,0,0.6431,0,0.2824,0.3686,0,0.5569,0.0549,0,0,0,0,0,0.0471,0.4,0.4627,0.251,0.7765,0.0627,0.3843,0,0,0.0706,0,0,0,0,0,0,0.0706,0,0,0.7529,0.0471,0.0157,0.1647,0.1569,0.1569,0.4863,0.502,0.0471,0.0314,0.4078,0.4627,0.2745,0.0235,0.0314,0,0.0314,0.0157,0,0.549,0.298,0,0,0.4314,0,0,0,0,0,0.0627,0,0,0,0,0.0627,0.6824,0,0.3373,0.4392,0.7137,0,0.2588,0.0235,0.149,0.0941,0,0,0.5176,0,0.6902,0.3529,0.5725,0.0314,0.2275,0.0627,0.251,0.0157,0.102,0.0784,0.0627,0,0,0.7373,0,0,0,0,0,0,0,0,0,0.0627,0,0,0.0627,0.1255,0.3137,0.1725,0.0235,0.4627,0.4627,0.6902,0.0627,0,0,0,0,0,0,0.149,0.149,0.149,0,0.251,0.0314,0.7686,0,0.0157,0.2353,0,0,0,0,0,0.0706,0.7765,0.5412,0.0314,0.4078,0.0314,0.5255,0.0627,0.0549,0.6902,0,0.7059,0,0,0.7137,0,0,0,0,0.0706,0.0549,0.6039,0.0392,0.3373,0.3137,0.3216,0.0235,0.1961,0.0078,0.1333,0,0.0706,0.0157,0,0.0549,0.0314,0.549,0.4549,0.0314,0.3686,0.3529,0.3294,0.3608,0.4941,0.4784,0.0706,0.0706,0.6824,0.6118,0.5804,0.4314,0.0392,0.4941,0.4235,0.0471,0.3529,0.0078,0.0392,0.0078,0.0392,0,0.7373,0.5647,0,0,0.5725,0,0,0.0471,0,0,0.5804,0,0,0.3686,0,0,0,0,0,0,0,0.0549,0.5961,0.0314,0.0314,0.0314,0.0471,0.6039,0.0549,0,0,0,0,0,0,0,0,0,0,0,0,0.0627,0.0549,0,0,0,0.0784,0,0,0,0,0,0.5098,0.5647,0.651,0.0706,0.0078,0.0157,0.3137,0.7373,0.1569,0.0078,0,0.3294,0.0078,0.3686,0.0627,0.6275,0,0,0.4784,0,0.6902,0.2745};
	//static int num_cols = 100;
	const vector<HE_Face*>& faces = m.getFacesRef();
	vector<HE_Face*>::const_iterator face_it = faces.begin();
	int c = 0;
	while(face_it != faces.end()) {
		HE_Face& f = *(*face_it);
		const set<HE_Vertex*> verts = f.getVertices();
		int n = verts.size();
		
		if(n == 3) {
			glBegin(GL_TRIANGLES);
		}
		else if(n == 4) {
			glBegin(GL_QUADS);
		}
		else {
			++face_it;
			continue;
		}

//		i = 0;
		HE_HalfEdge* he = f.getHalfEdge();
		Vec3 normal = f.getNormal();

		float vnum = 1;
		// @todo we need to add simplex noise to roxlu
		//hsv_to_rgb(col, snoise1(ofGetElapsedTimeMillis()*0.01),0.2,ofRandom(normal.x,normal.y));
		do {
			//ofSetColor(cols[i%cols.size()]);
			Vec3& v = he->getVertex()->getPositionRef();
			
			glColor3fv(col);
	//hsv_to_rgb(col, 0, 0.4, 1.0);
	
//			glColor3fv(&normal.x);
			glVertex3fv(&v.x);
			he = he->getNext();
			++vnum;
//			++i;
		} while(he != f.getHalfEdge());
		glEnd();		
		++face_it;
	}
}


	
		/*
			float* a = f_walk->vertices[0]->pt;
				float* b = f_walk->vertices[1]->pt;
				float* c = f_walk->vertices[2]->pt;
				glColor4f(fabs(a[0]/3.0),0.1,fabs(a[1]/20.0), 0.6);
				glColor4f(fabs(a[0]/2.0),0.6,fabs(a[1]/20.0), 0.6);
				glVertex3fv(a);
				glColor4f(fabs(b[0]/5.0),0.1,fabs(b[1]/5.0),1.0);
				glVertex3fv(b);
				glColor4f(fabs(c[0]/20.0),0.1,fabs(c[1]/20.0),0.4);
				glVertex3fv(c);	

		*/
	
		/*

		*/
		/*			
		set<HE_Vertex*>::const_iterator it = verts.begin();
		c = 0;
		while(it != verts.end()) {
			//float r = snoise2(c*0.01, c*0.04) * 2.5;
			//float g = snoise2(c*0.01, c*0.07) * 1.9;
			//float b = snoise2(c*0.01, c*0.03) * 1.5;
			Vec3& v = (*it)->getPositionRef();
//			float r = MAX(0.4,4*snoise1(v.x*0.01+c*100+ofGetElapsedTimef() * 0.1));
//			float g = MAX(0.4,4*snoise1(v.y*0.01+c*100+3+ofGetElapsedTimeMillis() * 0.001));
//			float b = MAX(0.4,4*snoise1(v.z*0.01+c*100+6+ofGetElapsedTimef() * 0.1));
			if(c%3 == 0) {
				glColor4f(fabs(v.x/2.0), 0.6, fabs(v.y/20.0), 0.6);
			}
			else if(c%3 == 1) {
				glColor4f(fabs(v.x/5.0), 0.1, fabs(v.y/5.0), 1.0);
			}
			else if(c%3 == 2) {
				glColor4f(fabs(v.x/20.0), 0.1, fabs(v.y/20.0), 0.4);
			}
			else {
				glColor4f(fabs(v.x/7.0), 0.8, fabs(v.y/20.0), 0.6);
			}
			
//			glColor3f(r,g,b);
			glVertex3fv(ptr3(v));
			++it;
			++c;
			//c+=0.1;
			
*/

// Draws the edges. Start vertex is green, end is red.
void HE_Debug::drawEdges(HE_Mesh& m) {
	const vector<HE_Edge*>& edges = m.getEdgesRef();
	vector<HE_Edge*>::const_iterator it = edges.begin();
	glBegin(GL_LINES);
	while(it != edges.end()) {
		HE_Edge& e = *(*it);
		glColor3f(0,1,0.2);
		glVertex3fv(&e.getStartVertex()->getPositionRef().x);
		
		glColor3f(1,0,0.2);
		glVertex3fv(&e.getEndVertex()->getPositionRef().x);
		++it;
	}
	glEnd();
}

// Draws outer edges from mesh using a HE_Selection
void HE_Debug::drawOuterEdges(HE_Mesh& m) {
	HE_Selection sel(&m);
	sel.addFaces(m.getFacesRef());
	sel.collectEdges();
	
	vector<HE_Edge*> outer_edges = sel.getOuterEdges();
	vector<HE_Edge*>::iterator it = outer_edges.begin();
	glColor3f(1,1,0);
	glBegin(GL_LINES);
	while(it != outer_edges.end()) {
		glVertex3fv(&(*it)->getStartVertex()->getPositionRef().x);
		glVertex3fv(&(*it)->getEndVertex()->getPositionRef().x);
		++it;
	}
	glEnd();
	glColor3f(1,1,1);
}

// Draws the inner edges from the mesh, using a HE_Selection.
void HE_Debug::drawInnerEdges(HE_Mesh& m) {
	HE_Selection sel(&m);
	sel.addFaces(m.getFacesRef());

	vector<HE_Edge*> inner_edges = sel.getInnerEdges();
	vector<HE_Edge*>::iterator it = inner_edges.begin();
	glColor3f(1,0.1,0.7);
	glBegin(GL_LINES);
	while(it != inner_edges.end()) {
		glVertex3fv(&(*it)->getStartVertex()->getPositionRef().x);
		glVertex3fv(&(*it)->getEndVertex()->getPositionRef().x);
		++it;
	}
	glEnd();
	glColor3f(1,1,1);
}

void HE_Debug::drawOuterVertices(HE_Mesh& m) {
	HE_Selection sel(&m);
	sel.addFaces(m.getFacesRef());

	set<HE_Vertex*> outer_verts = sel.getOuterVertices();
	set<HE_Vertex*>::iterator it = outer_verts.begin();
	glPointSize(5);
	glColor3f(1,1,0);
	glBegin(GL_POINTS);
	while(it != outer_verts.end()) {
		glVertex3fv(&(*it)->getPositionRef().x);
		++it;
	}
	glEnd();
	glPointSize(1);
	glColor3f(1,1,1);
}


void HE_Debug::drawInnerVertices(HE_Mesh& m) {
	HE_Selection sel(&m);
	sel.addFaces(m.getFacesRef());

	set<HE_Vertex*> inner_verts = sel.getInnerVertices();
	set<HE_Vertex*>::iterator it = inner_verts.begin();
	glPointSize(5);
	glColor3f(1,0.3,0.4);
	glBegin(GL_POINTS);
	while(it != inner_verts.end()) {
		glVertex3fv(&(*it)->getPositionRef().x);
		++it;
	}
	glEnd();
	glPointSize(1);
	glColor3f(1,1,1);
}


// STATISTICS
// -----------------------------------------------------------------------------
void HE_Debug::printStats(HE_Mesh& m) {
	vector<HE_HalfEdge*> unpaired = m.getUnpairedHalfEdges();
	cout << setw(25) << "Num vertices" 				<< setw(6) 	<< m.getNumVertices()	<< endl;
	cout << setw(25) << "Num faces" 				<< setw(6)  << m.getNumFaces() 		<< endl;	
	cout << setw(25) << "Num half edges"			<< setw(6) 	<< m.getNumHalfEdges()  << endl;
	cout << setw(25) << "Num <unpaired> half edges"	<< setw(6) 	<< unpaired.size() 		<< endl;
}

void HE_Debug::printVertexLabels(HE_Mesh& m) {
	const vector<HE_Vertex*>& verts = m.getVerticesRef();
	vector<HE_Vertex*>::const_iterator it = verts.begin();
	while(it != verts.end()) {
		cout << "Vertex: " << *it << " has label: " << (*it)->getLabel() << endl;
		++it;
	}
}

}; // roxlu