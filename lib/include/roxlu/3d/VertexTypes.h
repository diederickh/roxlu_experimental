#ifndef ROXLU_VERTEXTYPESH
#define ROXLU_VERTEXTYPESH

#include <vector>
#include <roxlu/math/Vec3.h>
#include <roxlu/math/Vec4.h>
#include <roxlu/math/Vec2.h>
#include <roxlu/graphics/Color.h>

using std::vector;

namespace roxlu {

// Good read: http://www.opengl.org/wiki/Tutorial1:_Rendering_shapes_with_glDrawRangeElements,_VAO,_VBO,_shaders_%28C%2B%2B_/_freeGLUT%29
	
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

struct VertexIndices {
	void add(unsigned int dx) {
		indices.push_back(dx);
	}
		
	size_t numBytes() {
		return indices.size() * sizeof(unsigned int);
	}
	
	size_t size() {
		return indices.size();
	}
	
	const unsigned int* getPtr() {
		return &indices[0];
	}
	
	vector<unsigned int> indices;
};

struct Vertex {
};

// @todo, add padding for memory alignment
struct VertexP : public Vertex {
	Vec3 pos;
	
	VertexP() 
		:pos(0,0,0)
	{
	}
	
	VertexP(const Vec3& v) 
		:pos(v.x, v.y, v.z)
	{
		
	}
	
	VertexP(const float& x, const float& y, const float& z) 
		:pos(x,y,z)
	{
	}
	
	void setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
};

// @todo, add padding for memory alignment
struct VertexPT : public Vertex  {
	Vec3 pos;
	Vec2 tex;
	
	VertexPT() {
	}
	
	VertexPT(const Vec3& pos, const Vec2& tex)	
		:pos(pos)
		,tex(tex)
	{
	
	}
	VertexPT(const Vec3& pos, const float u, const float v)
		:pos(pos)
		,tex(u,v)
	{
		
	}

	VertexPT(const float x, const float y, const float z, const float u, const float v) 
		:pos(x,y,z)
		,tex(u,v)
	{
		pos.set(x,y,z);
		tex.set(u,v);
	}

	void setPos(Vec3 p) {
		pos = p;
	}

	void setPos(const float x, const float y, const float z) {
		pos.set(x,y,z);
	}
	
	void setTex(const float u, const float v) {
		tex.set(u,v);
	}
	
	void set(const float x, const float y, const float z, const float u, const float v) {
		pos.set(x,y,z);
		tex.set(u,v);
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
};

// @todo, add padding for memory alignment
struct VertexPC : public Vertex {
	Vec3 pos;
	Vec4 col;
	
	VertexPC() {
	}
	
	VertexPC(const Vec3& pos, const Vec4& col)
		:pos(pos)
		,col(col)
	{
		
	}
	
	VertexPC(const float& x, const float& y, const float& z, const float& r, const float& g, const float& b, const float& a) {
		pos.set(x,y,z);
		col.set(r,g,b,a);
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
	
	VertexPC& setCol(const Vec4& c) {
		col = c;
		return *this;
	}
	
	VertexPC& setCol(const float& r, const float& g, const float& b, const float& a) {
		col.set(r,g,b,a);
		return *this;	
	}
	
	VertexPC& setPos(const Vec3& p) {
		pos = p;
		return *this;
	}
	
	VertexPC& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPC& set(const Vec3& p, const Vec3& c) {
		pos = p;
		col = c;
		return *this;
	}
};

// @todo, add padding for memory alignment
struct VertexPN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	
	
	VertexPN() {
	}
	
	VertexPN(const Vec3& pos, const Vec3& norm)
		:pos(pos)
		,norm(norm)
	{
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
	
	VertexPN& set(const Vec3& p, const Vec3& n) {
		pos = p;
		norm = n;
		return *this;
	}
	
	VertexPN& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPN setPos(const Vec3& p) {
		pos = p;
		return *this;
	}
	
	VertexPN& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPN& setNorm(const Vec3& n) {
		norm = n;
		return*this;
	}

};

// @todo, add padding for memory alignment
struct VertexPNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Vec4 col;
	
	VertexPNC() {
	}
	
	VertexPNC(const Vec3& p, const Vec3& n, const Vec4& c) 
		:pos(p)
		,norm(n)
		,col(c)
	{
	
	}
	
	VertexPNC& setNorm(const Vec3& n) {
		return setNorm(n.x, n.y, n.z);
	}
	
	VertexPNC& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPNC& setPos(const Vec3& p) {
		return setPos(p.x, p.y, p.z);
	}
	
	VertexPNC& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPNC& setCol(const float& r, const float& g, const float& b, const float& a) {
		col.set(r,g,b,a);
		return *this;
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
};


// @todo, add padding for memory alignment
struct VertexPTN : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	
	VertexPTN() {
	}
	
	VertexPTN(const Vec3& p, const Vec2& t, const Vec3& n) 
		:pos(p)
		,tex(t)
		,norm(n)
	{
	}
	
	VertexPTN(
		 const float& x, const float& y, const float& z
		,const float& nx, const float& ny, const float& nz
		,const float& s, const float& t
	)
	{
		pos.set(x,y,z);
		norm.set(nx,ny,nz);
		tex.set(s,t);
	}
	
	VertexPTN& setPos(const Vec3& p) {
		return setPos(p.x, p.y, p.z);
	}
	
	VertexPTN& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	VertexPTN& setNorm(const Vec3& n) {
		return setNorm(n.x, n.y, n.z);
	}
	
	VertexPTN& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPTN& setTex(const Vec2& t) {
		return setTex(t.x, t.y);
	}
	
	VertexPTN& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}
	
	const float* getPtr() {
		return pos.getPtr();
	}
};

// @todo, add padding for memory alignment
struct VertexPTNC : public Vertex  {
	Vec3 pos;
	Vec3 norm;
	Color4 col;
	Vec2 tex;
	
	VertexPTNC& setPos(const Vec3& p) {
		return setPos(p.x, p.y, p.z);
	}
	
	VertexPTNC& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	
	VertexPTNC& setNorm(const Vec3& n) {
		return setNorm(n.x, n.y, n.z);
	}
	
	
	VertexPTNC& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	VertexPTNC& setCol(const float& r, const float& g, const float& b, const float& a) {
		col.set(r,g,b,a);
		return *this;
	}
	
	VertexPTNC& setTex(const Vec2& t) {
		return setTex(t.x, t.y);
	}
	
	VertexPTNC& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}
};

// @todo, add padding for memory alignment
struct VertexPTNT : public Vertex  { // with tangent.
	Vec3 pos;
	Vec3 norm;
	Vec4 tan;
	Vec2 tex;
};

// @todo, add padding for memory alignment
struct VertexPTNTB : public Vertex  { // can be used for normal mapping
	Vec3 pos;
	Vec3 norm;
	Vec2 tex;
	Vec3 tan;
	Vec3 binorm;
	
	
	// POS
	VertexPTNTB& setPos(const Vec3& p) {
		return setPos(p.x, p.y, p.z);
	}
	
	VertexPTNTB& setPos(const float& x, const float& y, const float& z) {
		pos.set(x,y,z);
		return *this;
	}
	
	
	// NORM
	VertexPTNTB& setNorm(const Vec3& n) {
		return setNorm(n.x, n.y, n.z);
	}
	
	VertexPTNTB& setNorm(const float& x, const float& y, const float& z) {
		norm.set(x,y,z);
		return *this;
	}
	
	// TAN
	VertexPTNTB& setTan(const Vec3& t) {
		return setTan(t.x, t.y, t.z);
	}
	
	VertexPTNTB& setTan(const float& x, const float& y, const float& z) {
		tan.set(x,y,z);
		return *this;
	}
	
	// BINORM
	VertexPTNTB& setBinorm(const Vec3& b) {
		return setBinorm(b.x, b.y, b.z);
	}
	
	VertexPTNTB& setBinorm(const float& x, const float& y, const float& z) {
		binorm.set(x,y,z);
		return *this;
	}
	
	// TEX	
	VertexPTNTB& setTex(const Vec2& t) {
		return setTex(t.x,t.y);
	}
	
	VertexPTNTB& setTex(const float& u, const float& v) {
		tex.set(u,v);
		return *this;
	}

	// GETPTR
	const float* getPtr() {
		return pos.getPtr();
	}

};

// @todo, add padding for memory alignment
template<class T>
struct Vertices_Template {

	typedef T element_type;
	
	// returns index
	int add(const T& el) {
		verts.push_back(el);
		return verts.size() - 1;
	}
	
	void clear() {
		verts.clear();
	}
	
	size_t size() {
		return verts.size();
	}
	
	size_t numBytes() {
		return sizeof(T) * size();
	}
	
	const float* getPtr() {
		return verts[0].getPtr();
	}
	
	typename vector<T>::iterator begin() {
		return verts.begin();
	}
	
	typename vector<T>::iterator end() {
		return verts.end();
	}
	
	T& operator[](const unsigned int dx) {
		return verts[dx];
	}
	
	void resize(const size_t& newSize) {
		verts.resize(newSize);
	}
	
	T& back() {
		return verts.back();
	}

	vector<T> verts;
};


// @todo, add padding for memory alignment
template <class T>
struct Vertices_TemplateP : public Vertices_Template<T> {

	using Vertices_Template<T>::add;
	
	
	int add(const float& x, const float& y, const float& z) {
		VertexP p;
		p.setPos(x, y, z);
		this->verts.push_back(p);
		return this->verts.size()-1;
	}
	
	int addRectangle(const float& x, const float& y, const float& w, const float& h) {
		// first triangle.
		add(x  , y  , 0);
		add(x+w, y  , 0);
		add(x+w, y+h, 0);
		
		// second triangle.
		add(x  , y  , 0);
		add(x+w, y+h, 0);
		add(x  , y+h, 0); 
		return 6; // number of vertices created
	}
};

template<class T>
struct Vertices_TemplatePT : public Vertices_Template<T> {

	using Vertices_Template<T>::add;

	int add(const float& x, const float& y, const float& z, const float& u, const float& v) {
		VertexPT p;
		p.setPos(x, y, z);
		p.setTex(u,v);
		this->verts.push_back(p);
		return this->verts.size()-1;
	}

	// X,Y are bottom left coordinates
	const int addRectangle(const float& x, const float& y, const float& w, const float& h, bool flipUV = true) {
		if(flipUV) {
			add(x, y, 0, 0, 1);
			add(x+w, y, 0, 1, 1);
			add(x+w, y+h, 0, 1, 0);
			
			add(x+w, y+h, 0, 1, 0);
			add(x, y+h, 0, 0, 0);
			add(x, y, 0, 0, 1);
		}
		else {
			add(x, y, 0, 0, 0);
			add(x+w, y, 0, 1, 0);
			add(x+w, y+h, 0, 1, 1);
			
			add(x+w, y+h, 0, 1, 1);
			add(x, y+h, 0, 0, 1);
			add(x, y, 0, 0, 0);		
		}
		return 6;
	}

};

template<class T>
struct Vertices_TemplatePN : public Vertices_Template<T> {

	using Vertices_Template<T>::add;
	
	int add(const float* f) {
		return this->add(f+0, f+1, f+2, f+3, f+4, f+5);	
	}
	
	int add(const float& vx, const float& vy, const float& vz, const float& nx, const float& ny, const float& nz) {
		VertexPN p;
		p.setPos(vx, vy, vz);
		p.setNorm(nx, ny, nz);
		this->verts.push_back(p);
		return this->verts.size()-1;
	}
};

template<class T>
struct Vertices_TemplatePTN : public Vertices_Template<T> {
	using Vertices_Template<T>::add;
};

template<class T>
struct Vertices_TemplatePTNTB : public Vertices_Template<T> {
	using Vertices_Template<T>::add;
}; 

template<class T>
struct Vertices_TemplatePC : public Vertices_Template<T> {
	using Vertices_Template<T>::add;
};

typedef Vertices_TemplateP<VertexP>	VerticesP;
typedef Vertices_TemplatePC<VertexPC> VerticesPC;
typedef Vertices_TemplatePN<VertexPN> VerticesPN;
typedef Vertices_TemplatePN<VertexPNC> VerticesPNC;
typedef Vertices_TemplatePT<VertexPT> VerticesPT;
typedef Vertices_TemplatePTN<VertexPTN> VerticesPTN;
typedef Vertices_TemplatePTNTB<VertexPTNTB> VerticesPTNTB;



} // roxlu

#endif
