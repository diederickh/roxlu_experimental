#ifndef ROXLU_VERTEXTYPESH
#define ROXLU_VERTEXTYPESH

#include "Vec4.h"
#include "Vec3.h"
#include "Vec2.h"
#include "../Graphics/Color.h"

namespace roxlu {

// Here we define a couple of different vertex types; 
// P = position
// N = normal
// T = texcoord 
// T = tangent  (see member name)
// B = binormal
// C = color
enum VertexAttrib {
	 VERT_NONE 		= ( 0 )
	,VERT_POS 		= ( 1 << 0 )
	,VERT_TEX 		= ( 1 << 1 )
	,VERT_NORM		= ( 1 << 2 )
	,VERT_COL		= ( 1 << 3 )
	,VERT_BINORM 	= ( 1 << 4 )
	,VERT_TAN		= ( 1 << 5 )
	
	// clienstate types
	,ARRAY_VERT		= (1 << 6)
	,ARRAY_NORM		= (1 << 7)
	,ARRAY_COL		= (1 << 8)
	,ARRAY_TEX		= (1 << 9)
	,ARRAY_INDEX	= (1 << 10)
};

struct Vertex {
};

struct VertexP : public Vertex {
	Vec3 pos;
	
	void setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
	}
};

struct VertexPT : public Vertex  {
	Vec3 pos;
	Vec2 tex;
	
	VertexPT& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPT& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}
};

struct VertexPN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	
	VertexPN& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPN& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
};

struct VertexPNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Color4 col;
	
	VertexPNC& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPNC& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPNC& setCol(const float& r, const float& g, const float& b, const float& a) {
		col.set(r,g,b,a);
		return *this;
	}
};

struct VertexPTN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	
	
	VertexPTN& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPTN& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPTN& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}
};

struct VertexPTNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Color4 col;
	Vec2 tex;
	
	VertexPTNC& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	VertexPTNC& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	VertexPTNC& setCol(const float& r, const float& g, const float& b, const float& a) {
		col.set(r,g,b,a);
		return *this;
	}
	VertexPTNC& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}
};

struct VertexPTNT : public Vertex  { // with tangent.
	Vec3 pos;
	Vec3 norm;
	Vec4 tan;
	Vec2 tex;
};

struct VertexPTNTB : public Vertex  { // can be used for normal mapping
	Vec3 pos;
	Vec3 norm;
	Vec4 tan;
	Vec3 binorm;
	Vec2 tex;
};

} // roxlu

#endif