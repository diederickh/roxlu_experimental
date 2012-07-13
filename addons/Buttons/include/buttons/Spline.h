#ifndef ROXLU_BSPLINEH
#define ROXLU_BSPLINEH

#include <buttons/Types.h>
#include <buttons/Element.h>

namespace buttons {

struct CurvePoint {
	float x,y;
	CurvePoint():x(0),y(0) {}
	CurvePoint(const float x, const float y):x(x),y(y) {}
};


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// It would probably be cleaner to let the "user" of this gui, define it's spline
// ... but for now this will do.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Cubic Bezier Spline.
 *
 * Points are added in this order:
 * 
 * Point 0: first point
 * Point 1: first handle
 * Point 2: second handle <----
 * Point 3: second point
 *
 * Everything is normalized between [0,1]
 */
struct Spline {
	Spline();
	~Spline();
	void createCurves(int num); // resets points!
	bool at(float t, float& x, float& y);
		
	std::vector<CurvePoint> points;
};

inline bool Spline::at(float t, float& x, float& y) {
	if(points.size() < 4) {
		return false;
	}
	if(t > 1.0f) {
		return false;
	}

	float curve_p = t * (points.size()/4);
	int curve_num = curve_p;
	t = curve_p - curve_num;

	int dx = curve_num * 4;
	CurvePoint& p0 = points[dx]; // a
	CurvePoint& p1 = points[dx + 1]; // b 
	CurvePoint& p2 = points[dx + 2]; // c
	CurvePoint& p3 = points[dx + 3]; // d 

	CurvePoint a,b,c,p;
	
	c.x = 3 * (p1.x - p0.x);
	c.y = 3 * (p1.y - p0.y);
	b.x = 3 * (p2.x - p1.x) - c.x;
	b.y = 3 * (p2.y - p1.y) - c.y;
	a.x = p3.x - p0.x - c.x - b.x;
	a.y = p3.y - p0.y - c.y - b.y;

	float t2 = t*t;
	float t3 = t*t*t;
	x = a.x * t3 + b.x * t2 + c.x * t + p0.x;
	y = a.y * t3 + b.y * t2 + c.y * t + p0.y;
	return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Experimental...
// C: container for curve values: vector<Vec2>, vector<CurvePoint> etc..
template<class C> 
class SplineEditor_Template : public Element {
public:	
	SplineEditor_Template(const string& name, C& spline);
	~SplineEditor_Template();

	void setup();
	void draw(Shader& shader, const float* pm);
	
	void generateStaticText(Text& txt);
	void updateTextPosition(Text& staticText, Text& dynamicText);
	void generateVertices(ButtonVertices& shapeVertices);
	
	void onMouseDown(int mx, int my);
	void onMouseUp(int mx, int my);
	void onMouseEnter(int mx, int my);
	void onMouseLeave(int mx, int my);	
	void onMouseClick(int mx, int my);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();
	
	int label_dx;
	C& spline;
};


template<class C> 
SplineEditor_Template<C>::SplineEditor_Template(const string& name, C& spline) 
	:Element(BTYPE_SPLINE, name)
	,spline(spline)
	,label_dx(0)
{
	this->h = 80;	
}

template<class C> 
SplineEditor_Template<C>::~SplineEditor_Template() {
}

template<class C> 
void SplineEditor_Template<C>::generateStaticText(Text& txt) {
	this->label_dx = txt.add(this->x+4, this->y+2, this->label,  0.9, 0.9, 0.9, 0.9);
}

template<class C> 
void SplineEditor_Template<C>::updateTextPosition(Text& staticText, Text& dynamicText) {
	staticText.setTextPosition(this->label_dx, this->x+4, this->y+2);
}

template<class C> 
void SplineEditor_Template<C>::generateVertices(ButtonVertices& vd) {
	this->num_vertices = buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color);
	
	// add a  simple line
	vector<ButtonVertex> spline_verts;
	float col[4] = {0.9, 0.0, 0.0, 1.0};
//	spline_verts[0].set(this->x, this->y, col);
//	spline_verts[1].set(this->x+10, this->y+10, col); 
//	vd.add(spline_verts, GL_LINES);
	
	
	int num = 50;
	float spline_x, spline_y = 0;
	float padding_x = 4;
	float start_y = this->y + this->h * 0.5;
	float start_x = this->x + padding_x + 2;
	float space_x = this->w - (padding_x * 2); // 4 padding on both sides
	
	for(int i = 0; i <= num; ++i) {
		float p = float(i)/num;
		if(spline.at(p, spline_x, spline_y)) {
			spline_verts.push_back(ButtonVertex(
				 start_x + spline_x * space_x
				,start_y + spline_y
				,col
			));
			//printf("%f, %f, %f, %f\n", spline_verts.back().col[0], spline_verts.back().col[1],spline_verts.back().col[2] ,spline_verts.back().col[3]);
			/*
			glVertex2f(
				 start_x + spline_x * space_x
				,start_y + spline_y
			);
			*/
		}
	}
	vd.add(spline_verts, GL_LINE_STRIP);

	BSET_COLOR(col, 0.0,0.8,0.2,1.0);
	spline_verts.clear();
	//printf("%zu\n", spline.points.size());
	float handle_x = 0;
	float handle_y = 0;
	float handle_s = 5.0f;
	float handle_hs = handle_s * 0.5;
	for(int i = 0; i < spline.points.size(); ++i) {	
		int multi = 1;
		if(i == spline.points.size()-1) {
			multi = -1;
		}
		handle_x = start_x + spline.points[i].x * space_x - 4 + (handle_hs * multi);
		handle_y = start_y + spline.points[i].y - handle_hs;
		buttons::createRect(vd, handle_x, handle_y, handle_s, handle_s, col, col);
		//buttons::createRect(vd, this->x, this->y, this->w, this->h, this->bg_top_color, this->bg_bottom_color);
		//spline_verts.push_back(ButtonVertex(
		

	}
//	vd.add(spline_verts, GL_POINTS);

}

template<class C> 
void SplineEditor_Template<C>::setup() {
	if(spline.points.size() == 0) {
		spline.createCurves(2);
	}
}

template<class C> 
void SplineEditor_Template<C>::draw(Shader& shader, const float* pm) {
	
}


template<class C> 
void SplineEditor_Template<C>::onMouseDown(int mx, int my) {
}

template<class C> 
void SplineEditor_Template<C>::onMouseUp(int mx, int my) {
}

template<class C> 
void SplineEditor_Template<C>::onMouseEnter(int mx, int my) {
}

template<class C> 
void SplineEditor_Template<C>::onMouseLeave(int mx, int my) {
}

template<class C> 
void SplineEditor_Template<C>::onMouseClick(int mx, int my) {
}

template<class C> 
bool SplineEditor_Template<C>::canSave() {
	return false;
}

template<class C> 
void SplineEditor_Template<C>::save(std::ofstream& ofs) {
}

template<class C> 
void SplineEditor_Template<C>::load(std::ifstream& ifs) {
}


typedef SplineEditor_Template<Spline> SplineEditor;


} // namespace buttons

#endif