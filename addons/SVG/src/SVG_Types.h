#ifndef ROXLU_SVG_TYPESH
#define ROXLU_SVG_TYPESH

enum SVG_Type {
 	 SVG_LINE
	,SVG_POLYGON
	,SVG_CIRCLE
};

struct SVG_Point {
	SVG_Point();
	SVG_Point(const float& x, const float& y);
	SVG_Point(const float* p);
	float x,y;
};

inline SVG_Point::SVG_Point():x(0),y(0) {
}

inline SVG_Point::SVG_Point(const float& x, const float& y):x(x),y(y) {
}

inline SVG_Point::SVG_Point(const float* p):x(p[0]),y(p[1]) {
}

#endif