#ifndef ROXLU_TRIANGLEH
#define ROXLU_TRIANGLEH

//#include "Vec3.h"

namespace roxlu {

enum TriangleFlags {
	 TRI_VERT_INDICES 	= (1 << 0)
	,TRI_NORM_INDICES 	= (1 << 1)
	,TRI_TAN_INDICES 	= (1 << 2)
	,TRI_BIN_INDICES 	= (1 << 3)
};

struct Triangle {
	int va, vb, vc;	 // vertex indices
	int na, nb, nc;  // normal indices
	int ta, tb, tc;  // tangent indices
	int ba, bb, bc;  // binormal indices
	int tc_a, tc_b, tc_c; // texcoord indices
	int flags;	
	
	Triangle()
		:va(0),vb(0),vc(0)
		,na(0),nb(0),nc(0)
		,ta(0),tb(0),tc(0)
		,ba(0),bb(0),bc(0)
		,flags(0)
	{
	}
	
	Triangle(int vA, int vB, int vC)
		:va(vA),vb(vB),vc(vC)
		,na(0),nb(0),nc(0)
		,ta(0),tb(0),tc(0)
		,ba(0),bb(0),bc(0)
		,tc_a(0), tc_b(0), tc_c(0)
	{
        flags |= TRI_VERT_INDICES;
	}
	
	
	
	void setVertex(const int& a, const int& b, const int& c) {
		va = a;
		vb = b;
		vc = c;
       flags |= TRI_VERT_INDICES;
	}
	
	void setNormal(const int& a, const int& b, const int& c) {
		na = a;
		nb = b;
		nc = c;
		flags |= TRI_NORM_INDICES;
	}
	
	void setBinormal(const int& a, const int& b, const int& c) {
		ba = a;
		bb = b;
		bc = c;
		flags |= TRI_BIN_INDICES;
	}
	
	void setTangent(const int& a, const int& b, const int& c) {
		ta = a;
		tb = b;
		tc = c;
		flags |= TRI_TAN_INDICES;
	}
	
	void setTexCoord(const int& a, const int& b, const int& c) {
		tc_a = a;
		tc_b = b;
		tc_c = c;
	}
	
	bool hasNormals() {
		return (flags & TRI_NORM_INDICES);
	}
	
	bool hasTangents() {
		return (flags & TRI_TAN_INDICES);
	}
	
	bool hasBinormals() {
		return (flags & TRI_BIN_INDICES);
	}	
	
	
	int& operator[](int n){
		return (&va)[n];
	}
};

} // roxlu
#endif