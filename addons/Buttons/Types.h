#ifndef ROXLU_RTYPESH
#define ROXLU_RTYPESH

#include "Vec2.h"
#include <vector>

										//((x>=i)   && (x <=(i +w)) && (y >= j) && y <= (j+h))
#define BMOUSE_INSIDE(mx,my,ex,ey,w,h) 	(mx>=ex && mx<=(ex+w) && my >= ey && my <= (ey+h))
#define BINSIDE_ELEMENT(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->h)
#define BINSIDE_HEADER(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->hh)
#define BSET_COLOR(col, r, g, b, a) col[0] = r; col[1] = g; col[2] = b; col[3] = a;
#define BSET_MAT_POS(mat, x, y, z) mat[11] = x; mat[12] = y; mat[13] = z;

using std::vector;
using roxlu::Vec2;

namespace buttons {

class ButtonVertex {

public:

	float pos[2];
	float col[4];
	
	void setPos(const float& x, const float& y) {
		pos[0] = x;
		pos[1] = y;
	}
	
	void setCol(const float& r, const float& g, const float& b, const float& a) {
		col[0] = r;
		col[1] = g;
		col[2] = b;
		col[3] = a;
	}
};

class ButtonVertices {
public:
	ButtonVertices() {
	}
	
	int add(const float& x, const float& y, const float* col4) {
		return add(x,y,col4[0], col4[1], col4[2], col4[3]);
	}
	
	int add(const float& x, const float& y, const float& r, const float& g, const float& b, const float& a) {
		ButtonVertex v;
		v.setPos(x,y);
		v.setCol(r,g,b,a);
		verts.push_back(v);
		return verts.size()-1;
	}
	
	size_t size() {
		return verts.size();
	}
	
	float* getPtr() {
		return &verts[0].pos[0];
	}
	
	void clear() {
		verts.clear();
	}
	
	ButtonVertex& operator[](const int& i) {
		return verts[i];
	}
	
	vector<ButtonVertex> verts;
};

static int createRect(ButtonVertices& vd, int x, int y, int w, int h, float* topCol, float* botCol) {
	vd.add(x,y,topCol);
	vd.add(x+w,y,topCol);
	vd.add(x+w,y+h,botCol);
	
	vd.add(x+w,y+h,botCol);
	vd.add(x,y+h,botCol);
	vd.add(x,y,topCol);
	return 6; // number of vertices
}

} // namespace buttons


#endif