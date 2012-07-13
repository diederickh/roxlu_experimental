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

// just a helper...
struct EditorCoords {
	EditorCoords()
		:min_x(0)
		,min_y(0)
		,max_x(0)
		,max_y(0)
		,w(0)
		,h(0)
		,handle_s(5.0f)
		,handle_hs(2.5)
		,max_curve_w(0.0f)
		,max_curve_h(0.0f)
	{
	}
	
	float min_x;
	float min_y;
	float max_x;
	float max_y;
	float w;
	float h;
	float handle_s;
	float handle_hs;
	float max_curve_h;
	float max_curve_w;
	
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// It would probably be cleaner to let the "user" of this gui, define it's spline
// ... but for now this will do.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 * Catmull Rom interpolation. 
 * --------------------------
 * Catmull Rom interpolation works with 4 points, there the 
 * local "t" value is used to interpolate between points B and C. The 
 * points A and D are used as "direction" (kind of). Therefore, for the first
 * and last point we have to choose the indices correctly. (see the index
 * checking for a,b,c,d below.). Basically for the first point, we use 0,0
 * for points A and Bs.
 *
 * Everything is normalized between [0,1]
 */
struct Spline {
	Spline();
	~Spline();
	void createCurves(int num); // resets points!
	bool at(float t, float& x, float& y);
	void add(const float x, const float y);
	std::vector<CurvePoint> points;
};

inline bool Spline::at(float t, float& x, float& y) {
	if(points.size() < 4) {
		return false;
	}
	if(t > 0.999f) {
		//return false;
		t = 0.99f;
	}
	else if(t < 0) {
		t = 0;
	}

	// get local "t" (also mu)	
	float curve_p = t * (points.size()-1);
	int curve_num = curve_p;
	t = curve_p - curve_num; // local t
	
	// get the 4 point
	int b = curve_num;
	int a = b - 1;
	int c = b + 1;
	int d = c + 1;
	if(a < 0) {
		a = 0;
	}
	if(d >= points.size()) {
		d = points.size()-1;
	}
	CurvePoint& p0 = points[a]; // a
	CurvePoint& p1 = points[b]; // b 
	CurvePoint& p2 = points[c]; // c
	CurvePoint& p3 = points[d]; // d 

	
	float t2 = t*t;
	float t3 = t*t*t;

	// Catmull interpolation	
	x = 0.5 * ((2 * p1.x) + (-p0.x + p2.x) * t + (2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x) * t2 + (-p0.x + 3 * p1.x - 3 * p2.x + p3.x) * t3);
	y = 0.5 * ((2 * p1.y) + (-p0.y + p2.y) * t + (2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y) * t2 + (-p0.y + 3 * p1.y - 3 * p2.y + p3.y) * t3);	
	

	/*
	// Cubic interpolation
	int num_curves = 1 + (float(points.size())-4.0f)/3.0f;
	float curve_p = t * num_curves;
	int curve_num = curve_p;
	t = curve_p - curve_num;
	
	int end_dx = 3 + (curve_num-1) * 3; // [num_start_points] + (curvenum-1) * [num_points_successive_curves], we remove 1 from the start, which is 4 so we have a array index

	int dx_d = end_dx+3;
	int dx_c = end_dx+2;
	int dx_b = end_dx+1;
	int dx_a = end_dx;

	int dx = end_dx;
	CurvePoint& p0 = points[dx]; // a
	CurvePoint& p1 = points[dx + 1]; // b 
	CurvePoint& p2 = points[dx + 2]; // c
	CurvePoint& p3 = points[dx + 3]; // d 

	CurvePoint a0,a1, a2, a3;
	float t2 = t*t;
	a0.x = p3.x - p2.x - p0.x + p1.x;
	a0.y = p3.y - p2.y - p0.y + p1.y;
	a1.x = p0.x - p1.x - a0.x;
	a1.y = p0.y - p1.y - a0.y;
	a2.x = p2.x - p0.x;
	a2.y = p2.y - p0.y;
	a3.x = p1.x;
	a3.y = p1.y;

	x = a0.x * t * t2 + a1.x * t2 + a2.x * t + a3.x;
	y = a0.y * t * t2 + a1.y * t2 + a2.y * t + a3.y;
	*/
	
	/*
	*/
	
	
	/*
	// This Cubic Bezier spline, works with a set of points where points
	// and handles are added in a successive way: point, handle, handle, point
	// handle, handle, point, handle, handle, point, etc...
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int num_curves = 1 + (float(points.size())-4.0f)/3.0f;
	float curve_p = t * num_curves;
	int curve_num = curve_p;
	t = curve_p - curve_num;
	
	int end_dx = 3 + (curve_num-1) * 3; // [num_start_points] + (curvenum-1) * [num_points_successive_curves], we remove 1 from the start, which is 4 so we have a array index

	int dx_d = end_dx+3;
	int dx_c = end_dx+2;
	int dx_b = end_dx+1;
	int dx_a = end_dx;

	//printf("end dx: %d, (%d, %d, %d, %d)\n", end_dx, dx_a, dx_b, dx_c, dx_d);
	int dx = end_dx;
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
	*/
	
	
	
	
	//printf("Curve_p: %f, t: %f, local_t: %f\n", curve_p, t, lt);
	//printf("Num curves: %d, num_points: %zu\n", num_curves, points.size());
	
	
//	float curve_p = t * (4 + points.size()-4/3);
//	int curve_num = curve_p;

	/*
	// This version is an implementation of Cubic Beziers, where you we assume
	// that multiple curves are added. So everytime 4 points.
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
	*/
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
	void onMouseMoved(int mx, int my); // global move, using is_mouse_inside
	void onMouseDragged(int mx, int my, int dx, int dy);
	
	void save(std::ofstream& ofs);
	void load(std::ifstream& ifs);
	bool canSave();
	
	// (spline) helper functions
	void generateSplineVertices();
	int selected_handle_dx;
	float selected_handle_col[4];
	float editor_bg_bottom_col[4];
	float editor_bg_top_col[4];
	
	EditorCoords editor_coords;
	int label_dx;
	C& spline;
	vector<CurvePoint> screen_handles;
};


template<class C> 
SplineEditor_Template<C>::SplineEditor_Template(const string& name, C& spline) 
	:Element(BTYPE_SPLINE, name)
	,spline(spline)
	,label_dx(0)
	,selected_handle_dx(-1)
{
	this->h = 100;	
	BSET_COLOR(selected_handle_col, 1.0, 0.7,0.0,1.0);
	BSET_COLOR(editor_bg_bottom_col, 0.3, 0.3,0.3,1.0);
	BSET_COLOR(editor_bg_top_col, 0.1, 0.1,0.1,1.0);
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
	
	float hh = h * 0.3333;

	editor_coords.min_x = this->x;
	editor_coords.max_x = editor_coords.min_x + this->w;
	editor_coords.min_y = hh+this->y;
	editor_coords.max_y = editor_coords.min_y + hh;
	editor_coords.w = editor_coords.max_x - editor_coords.min_x;
	editor_coords.h = editor_coords.max_y - editor_coords.min_y;
	editor_coords.max_curve_h = editor_coords.h * 0.5;
	
			
//	editor_coords.min_x = this->x;
//	editor_coords.max_x = editor_coords.min_x + this->w;
//	editor_coords.min_y = this->y;
//	editor_coords.max_y = editor_coords.min_y + this->h;
//	editor_coords.w = editor_coords.max_x - editor_coords.min_x;
//	editor_coords.h = editor_coords.max_y - editor_coords.min_y;
//	editor_coords.max_curve_h = editor_coords.h * 0.5;
//	editor_coords.max_curve_w = editor_coords.w;

	
	buttons::createRect(vd, editor_coords.min_x, editor_coords.min_y, editor_coords.w, editor_coords.h, editor_bg_top_col, editor_bg_bottom_col);

	generateSplineVertices();
	
	//
	
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
	float space_y = h;
	for(int i = 0; i <= num; ++i) {
		float p = float(i)/num;
		if(spline.at(p, spline_x, spline_y)) {
			spline_verts.push_back(ButtonVertex(
				editor_coords.min_x + spline_x * editor_coords.w
			//	,editor_coords.max_y - spline_y * editor_coords.h
				,editor_coords.max_y - spline_y * editor_coords.h
			
				,col
			));
		}

		
//			spline_verts.push_back(ButtonVertex(
//				 start_x + spline_x * space_x
//				,start_y + spline_y * space_y 
//				,col
//			));
			//printf("%f, %f, %f, %f\n", spline_verts.back().col[0], spline_verts.back().col[1],spline_verts.back().col[2] ,spline_verts.back().col[3]);
			/*
			glVertex2f(
				 start_x + spline_x * space_x
				,start_y + spline_y
			);
			*/
		
	}
	vd.add(spline_verts, GL_LINE_STRIP);

	BSET_COLOR(col, 0.0,0.8,0.2,1.0);
	float* handle_col = col;


	for(vector<CurvePoint>::iterator it = screen_handles.begin(); it != screen_handles.end(); ++it) {
		CurvePoint& p = *it;
		buttons::createRect(vd, p.x - editor_coords.handle_hs, p.y  - editor_coords.handle_hs, editor_coords.handle_s, editor_coords.handle_s, handle_col, handle_col);
	}

	/*
	float* handle_col = col;
	spline_verts.clear();
	float handle_x = 0;
	float handle_y = 0;
	float handle_s = 5.0f;
	float handle_hs = handle_s * 0.5;
	for(int i = 0; i < spline.points.size(); ++i) {	
		int multi = 1;
		if(i == spline.points.size()-1) {
			multi = -1;
		}
		handle_col = (i == selected_handle_dx) ? selected_handle_col : col;
		handle_x = start_x + spline.points[i].x * space_x - 4 + (handle_hs * multi);
		handle_y = start_y + spline.points[i].y * space_y - handle_hs;
		buttons::createRect(vd, handle_x, handle_y, handle_s, handle_s, handle_col, handle_col);
	}
	*/
}

template<class C> 
void SplineEditor_Template<C>::setup() {
	if(spline.points.size() == 0) {
		spline.createCurves(1);
	}
}

template<class C> 
void SplineEditor_Template<C>::generateSplineVertices() {
//	if(screen_handles.size()) {
//		return;
//	}
	screen_handles.clear();
	float max_hx = editor_coords.max_x-editor_coords.handle_hs-2; // max handle x
	for(int i = 0; i < spline.points.size(); ++i) {	
		int multi = 1;
		if(i == spline.points.size()-1) {
			multi = -1;
		}
		//handle_x = start_x + spline.points[i].x * space_x - 4 + (handle_hs * multi);
		//handle_y = start_y + spline.points[i].y * space_y - handle_hs;
		//if(i == selected_handle_dx) {
		//	printf("%f, %f<--------\n", handle_x, handle_y);
		//}
		CurvePoint& p = spline.points[i];
		screen_handles.push_back(CurvePoint(
			std::max<float>(editor_coords.handle_s+2, std::min<float>(max_hx, editor_coords.min_x + (p.x * editor_coords.w)))
			,editor_coords.max_y - (p.y * editor_coords.h)
		));
	}
	
	
	// x/y/w 
	/*
	float padding_x = 4;
	float start_y = this->y + this->h * 0.5;
	float start_x = this->x + padding_x + 2;
	float space_x = this->w - (padding_x * 2); // 4 padding on both sides
	float space_y = h * 0.5;
	
	// generate the position (screen coords)
	float handle_x = 0;
	float handle_y = 0;
	float handle_s = 5.0f;
	float handle_hs = handle_s * 0.5;
	*/
//	for(int i = 0; i < spline.points.size(); ++i) {	
//		int multi = 1;
//		if(i == spline.points.size()-1) {
//			multi = -1;
//		}
//		handle_x = start_x + spline.points[i].x * space_x - 4 + (handle_hs * multi);
//		handle_y = start_y + spline.points[i].y * space_y - handle_hs;
//		if(i == selected_handle_dx) {
//			printf("%f, %f<--------\n", handle_x, handle_y);
//		}
//		
//		screen_handles.push_back(CurvePoint(handle_x, handle_y));
//	}

}


template<class C> 
void SplineEditor_Template<C>::draw(Shader& shader, const float* pm) {
	
}

template<class C> 
void SplineEditor_Template<C>::onMouseMoved(int mx, int my) {

	// find the handle which must be selected.
	float dist_sq = 225;
	float dx, dy, sq = 0;
	int sel = 0;
	int curr_sel = selected_handle_dx;
	selected_handle_dx = -1;
	if(is_mouse_inside) {	
		for(vector<CurvePoint>::iterator it = screen_handles.begin(); it != screen_handles.end(); ++it) {
			CurvePoint& p = *it;
			dx = (mx - p.x);
			dy = (my - p.y);
			sq = (dx * dx) + (dy*dy);
			
			if(sq < dist_sq) {
				selected_handle_dx = sel;
			}
			++sel;
		}
	}
	if(curr_sel != selected_handle_dx) {
		needsRedraw();		
	}
}

template<class C> 
void SplineEditor_Template<C>::onMouseDragged(int mx, int my, int dx, int dy) {
	if(drag_inside && selected_handle_dx != -1) {
		//printf("lets move, %d!\n", dy);
		CurvePoint& screen_point = screen_handles[selected_handle_dx];
		CurvePoint new_point(screen_point.x-dx, screen_point.y-dy);
		/*
		float space_x = this->w - (4 * 2);
		float px = float(dx)/1024.0f;
		float ppx = px * (float(space_x)/1024.0f);
		float py = float(dy)/768.0f;
		float ppy = py * (float(h)/768.0f);
//		spline.points[selected_handle_dx].y += (ppy * h);
//		spline.points[selected_handle_dx].x += dx * 0.007; //(ppx * h);
		printf("perc, py: %f\n", py);
		*/
		screen_point.x = float(mx) - editor_coords.handle_hs;
		screen_point.y = float(my) - editor_coords.handle_hs;
		float local_x = screen_point.x - editor_coords.min_x;
		float local_y = screen_point.y - editor_coords.min_y;
		float xp = local_x / editor_coords.w;
		float yp =  1.0f - (local_y / editor_coords.h);
		printf("%f, %f\n", xp,yp);
		spline.points[selected_handle_dx].x = std::max<float>(0.0f,std::min<float>(1.0f,xp));
		spline.points[selected_handle_dx].y = std::max<float>(0.0f,std::min<float>(1.0f,yp)); // measured
		needsRedraw();
	}
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