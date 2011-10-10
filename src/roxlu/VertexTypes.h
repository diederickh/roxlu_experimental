#pragma once
#include "Vec3.h"
#include "Vec2.h"
#include "Color.h"


// Here we define a couple of different vertex types; 
// P = position
// N = normal
// T = texcoord 
// T = tangent  (see member name)
// B = binormal
// C = color
enum VertexAttribs {
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
	,ARRAY_INDEX		= (1 << 10)
};

struct Vertex {
};

struct VertexP : public Vertex {
	Vec3 pos;
};

struct VertexPT : public Vertex  {
	Vec3 pos;
	Vec2 tex;
};

struct VertexPN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
};

struct VertexPNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Color4f col;
};

struct VertexPTN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
};

struct VertexPTNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Color4f col;
	Vec2 tex;
};

struct VertexPTNTB : public Vertex  { // can be used for normal mapping
	Vec3 pos;
	Vec3 norm;
	Vec3 tan;
	Vec3 binorm;
	Vec2 tex;
};
