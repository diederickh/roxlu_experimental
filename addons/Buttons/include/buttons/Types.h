#ifndef ROXLU_RTYPESH
#define ROXLU_RTYPESH

#include <roxlu/opengl/OpenGLInit.h>
#include <roxlu/math/Vec2.h>
#include <iterator>
#include <vector>
										//((x>=i)   && (x <=(i +w)) && (y >= j) && y <= (j+h))
#define BMOUSE_INSIDE(mx,my,ex,ey,w,h) 	(mx>=ex && mx<=(ex+w) && my >= ey && my <= (ey+h))
#define BINSIDE_ELEMENT(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->h)
#define BINSIDE_HEADER(ptr, mx, my)  BMOUSE_INSIDE(mx, my, ptr->x, ptr->y, ptr->w, ptr->header_h)
#define BSET_COLOR(col, r, g, b, a) col[0] = r; col[1] = g; col[2] = b; col[3] = a;
#define BSET_MAT_POS(mat, x, y, z) mat[11] = x; mat[12] = y; mat[13] = z;

using std::vector;
using roxlu::Vec2;

namespace buttons {

enum ElementTypes {
	 BTYPE_SLIDER 	= 0
	,BTYPE_TOGGLE 	= 1
	,BTYPE_BUTTON 	= 2
	,BTYPE_RADIO  	= 3
	,BTYPE_SPLINE 	= 4
};

enum ElementStates {
	 BSTATE_NONE
	,BSTATE_ENTER
	,BSTATE_LEAVE
};

enum ElementValueTypes {
	 BVALUE_NONE
	,BVALUE_INT
	,BVALUE_FLOAT
};

class ButtonVertex {

public:

	float pos[2];
	float col[4];
	
	ButtonVertex() {
		pos[0] = pos[1] = 0.0f;
		col[0] = col[1] = col[2] = col[3] = 0.0f;
	}
	
	ButtonVertex(const float x, const float y, const float* col4) {
		set(x,y,col4);
	}
	
	
	void setPos(const float x, const float y) {
		pos[0] = x;
		pos[1] = y;
	}
	
	void setCol(const float r, const float g, const float b, const float a) {
		col[0] = r;
		col[1] = g;
		col[2] = b;
		col[3] = a;
	}
	
	void set(const float x, const float y, const float* col4) {
		pos[0] = x;
		pos[1] = y;
		col[0] = col4[0];
		col[1] = col4[1];
		col[2] = col4[2];
		col[3] = col4[3];
	}
};

struct ButtonDrawArray {
	ButtonDrawArray(int m, int s, int c):mode(m),start(s),count(c){}
	int mode;
	int start;
	int count;
};

class ButtonVertices {
public:
	ButtonVertices() {
	}
	
	/*
	int add(const float& x, const float& y, const float* col4) {
		return add(x,y,col4[0], col4[1], col4[2], col4[3]);
	}

	int add(const float& x, const float& y, const float& r, const float& g, const float& b, const float& a) {
		printf("ADD CALLED\n");
		
		
		ButtonVertex v;
		v.setPos(x,y);
		v.setCol(r,g,b,a);
		verts.push_back(v);
		return verts.size()-1;
		
		
	}
	*/
	
	size_t size() {
		return verts.size();
	}
	
	float* getPtr() {
		return &verts[0].pos[0];
	}
	
	void clear() {
	//	printf("CLEAR CALLED?\n");
		draw_arrays.clear();
		verts.clear();
	}
	
	void add(vector<ButtonVertex>& newVertices, int drawMode) {
		int start = size();
		std::copy(newVertices.begin(), newVertices.end(), std::back_inserter(verts));
		draw_arrays.push_back(ButtonDrawArray(drawMode, start, newVertices.size()));
	//	printf("added: %zu\n", newVertices.size());
	}
	
	ButtonVertex& operator[](const int& i) {
	//	return tmp_verts[i];
		return verts[i];
	}
//	vector<ButtonVertex> tmp_verts; // testing
	vector<ButtonVertex> verts;
	vector<ButtonDrawArray> draw_arrays;
};

static int createRect(ButtonVertices& vd, int x, int y, int w, int h, float* topCol, float* botCol) {
	/*
	vd.add(x,y,topCol);
	vd.add(x+w,y,topCol);
	vd.add(x+w,y+h,botCol);
	
	vd.add(x+w,y+h,botCol);
	vd.add(x,y+h,botCol);
	vd.add(x,y,topCol);
	*/
	
	// new way of adding elements.
	vector<ButtonVertex> new_verts(6);
	new_verts[0].set(x,y,topCol);
	new_verts[1].set(x+w,y,topCol);
	new_verts[2].set(x+w,y+h,botCol);
	
	new_verts[3].set(x+w,y+h,botCol);
	new_verts[4].set(x,y+h,botCol);
	new_verts[5].set(x,y,topCol);
	vd.add(new_verts, GL_TRIANGLES);
	
	return 6; // number of vertices
}

} // namespace buttons


#endif