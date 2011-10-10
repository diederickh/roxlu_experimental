#include "VertexData.h"
VertexData::VertexData() 
:vertex_p(NULL)
,vertex_pn(NULL)
,vertex_ptn(NULL)
,vertex_pnc(NULL)
,vertex_ptntb(NULL)
,attribs(VERT_NONE)
{	
}

int VertexData::addVertex(const Vec3& rVec) {
	vertices.push_back(rVec);
	attribs |= VERT_POS;
	return vertices.size()-1;
}

int VertexData::addVertex(const float nX, const float nY, const float nZ) {
	vertices.push_back(Vec3(nX,nY,nZ));
	attribs |= VERT_POS;
	return vertices.size()-1;
}

void VertexData::addTexCoord(const Vec2& rVec) {
	texcoords.push_back(rVec);
	attribs |= VERT_TEX;
}

void VertexData::addTexCoord(const float nX, const float nY) {
	texcoords.push_back(Vec2(nX, nY));
	attribs |= VERT_TEX;
}

void VertexData::addColor(const Vec3& rColor) {
	addColor(Color4f(rColor.x, rColor.y, rColor.z, 1));
}

void VertexData::addColor(const float nR, const float nG, const float nB) {
	addColor(Color4f(nR, nG, nB, 1.0f));
}

void VertexData::addColor(const Color4f& rCol) {
	colors.push_back(rCol);
	attribs |= VERT_COL;
}

void VertexData::addNormal(const float nX, const float nY, const float nZ) {
	normals.push_back(Vec3(nX, nY, nZ));
	attribs |= VERT_NORM;
}

void VertexData::addNormal(const Vec3& rVec) {
	normals.push_back(rVec);
	attribs |= VERT_NORM;
}

void VertexData::addIndex(const int& rIndex) {
	indices.push_back(rIndex);
}

int VertexData::addTriangle(const int nA, const int nB, const int nC) {
	// keep track of triangle.
	Triangle t(nA, nB, nC);
	triangles.push_back(t);
	
	// also create indexed triangle.
	indices.push_back(nA);
	indices.push_back(nB);
	indices.push_back(nC);
	
	// return triangle index.
	return triangles.size()-1;
}

int VertexData::addQuad(int nA, int nB, int nC, int nD) {
	Quad q(nA, nB, nC, nD);
	quads.push_back(q);
	return quads.size()-1;
}

Triangle* VertexData::getTriangle(int nTriangle) {
	return &triangles[nTriangle];	
}

Quad* VertexData::getQuad(int nQuad) {
	return &quads[nQuad];
}


Vec3 VertexData::getVertex(int nIndex) {
	return vertices[nIndex];
}

Vec3* VertexData::getVertexPtr(int nIndex) {
	return &vertices[nIndex];
}

const float* VertexData::getVertices() {
	return &vertices[0].x;
}

const float* VertexData::getTexCoords() {
	return &texcoords[0].x;
}

const float* VertexData::getNormals() {
	return &normals[0].x;
}

const float* VertexData::getColors() {
	return &colors[0].r;
}

const int* VertexData::getIndices() {
	return &indices[0];
}

int VertexData::getNumVertices() {
	return (int)vertices.size();
}

int VertexData::getNumTexCoords() {
	return (int)texcoords.size();
}

int VertexData::getNumColors() {
	return (int)colors.size();
}

int VertexData::getNumNormals() {
	return (int)normals.size();
}

int VertexData::getNumIndices() {
	return (int)indices.size();
}

int VertexData::getNumTriangles() {
	return (int)triangles.size();
}

int VertexData::getNumQuads() {
	return (int)quads.size();
}


VertexP* VertexData::getVertexP() {
	if(vertex_p != NULL) { 	
		return vertex_p;
	}
	if( ! (attribs & VERT_POS) ) { 	
		return NULL; 
	}
	
	int num = getNumVertices();
	vertex_p = new VertexP[num];
	for(int i = 0; i < num; ++i) {	
		vertex_p[i].pos = vertices[i]; 
	}
	return vertex_p;
}

VertexPN* VertexData::getVertexPN() {
	if(vertex_pn != NULL) { 	
		return vertex_pn; 
	}
	if( ! (attribs & (VERT_POS | VERT_NORM)) ) { 	
		return NULL;  
	}
	
	int num = getNumVertices();
	vertex_pn = new VertexPN[num];
	for(int i = 0; i < num; ++i) {	
		vertex_pn[i].pos = vertices[i]; 
		vertex_pn[i].norm = normals[i]; 
	}
	return vertex_pn;
}

VertexPTN* VertexData::getVertexPTN() {
	if(vertex_ptn != NULL) { 	
		return vertex_ptn; 
	}
	if( ! (attribs & (VERT_POS | VERT_NORM | VERT_TEX)) ) { 	
		return NULL;  
	}
	
	int num = getNumVertices();
	vertex_ptn = new VertexPTN[num];
	for(int i = 0; i < num; ++i) {	
		vertex_ptn[i].pos = vertices[i]; 
		vertex_ptn[i].norm = normals[i]; 
		vertex_ptn[i].tex = texcoords[i]; 
	}
	return vertex_ptn;
}

VertexPNC* VertexData::getVertexPNC() {
	if(vertex_pnc != NULL) { 	
		return vertex_pnc; 
	}
	if( ! (attribs & (VERT_POS | VERT_NORM | VERT_COL)) ) { 	
		return NULL;  
	}
	
	int num = getNumVertices();
	vertex_pnc = new VertexPNC[num];
	for(int i = 0; i < num; ++i) {	
		vertex_pnc[i].pos = vertices[i]; 
		vertex_pnc[i].norm = normals[i]; 
		vertex_pnc[i].col = colors[i]; 
	}
	return vertex_pnc;
}

VertexPTNTB* VertexData::getVertexPTNTB() {
	if(vertex_ptntb != NULL) { 	
		return vertex_ptntb; 
	}
	if( ! (attribs & (VERT_POS | VERT_NORM | VERT_TEX | VERT_BINORM | VERT_TAN)) ) { 	
		return NULL;  
	}
	
	int num = getNumVertices();
	vertex_ptntb = new VertexPTNTB[num];
	for(int i = 0; i < num; ++i) {	
		vertex_ptntb[i].pos = vertices[i]; 
		vertex_ptntb[i].tex = texcoords[i]; 
		vertex_ptntb[i].norm = normals[i]; 
		vertex_ptntb[i].tan = tangents[i]; 
		vertex_ptntb[i].binorm = bitangents[i]; 
	}
	return vertex_ptntb;

}


/** 
 * Here we calculate the tangent and bitangents. We want the tangent and 
 * bitangent to be aligned with texture-space. From "Mathematics for 3D 
 * Game Programming and Computer Graphics", we use this to calculate the 
 * B/T:
 *
 * Q = arbitrary point in triangle
 * P0 = position of one of the vertices of a triangle
 * S,S0,T,T0 = texcoords (we align to this direction).
 * T = tangent vector (the one we want)
 * B = bitangent vector (the other we want).
 * 
 * Q - P0 = (S - S0)T + (T-T0)B
 * 
 * The book mentioned above shows how to solve this! Page 185.
 * 
 * references:
 * 
 * -	Good info on how to calculate the TB: 
 * 		http://www.blacksmith-studios.dk/projects/downloads/bumpmapping_using_cg.php
 *
 * -	Source code I used for this:
 *		http://www.3dkingdoms.com/weekly/weekly.php?a=37
 * 
 *
 * TODO: this method should be optimized as it loops over all triangles so
 * doing things twice or more for shared vertices.
 */
 
void VertexData::calculateTangentAndBiTangent() {
	if(triangles.size() == 0 || texcoords.size() == 0) {
		printf("Error: cannot calculate (bi)tangents as we do not have any triangles, or no texcoords defined.\n");
		return;
	}
	
	if(tangents.size() == 0 && bitangents.size() == 0) {
		Vec3 d(0,0,0);
		tangents.assign(getNumVertices(), d);
		bitangents.assign(getNumVertices(), d);
	}
	for(int i = 0; i < triangles.size(); ++i) {
		Vec3& v1 = vertices[triangles[i].a];
		Vec3& v2 = vertices[triangles[i].b];
		Vec3& v3 = vertices[triangles[i].c];				
		Vec3 edge1 = v2 - v1;
		Vec3 edge2 = v3 - v1;
		Vec2& uv1 = texcoords[triangles[i].a];
		Vec2& uv2 = texcoords[triangles[i].b];
		Vec2& uv3 = texcoords[triangles[i].c];
		Vec2 edge1_uv = uv2 - uv1;
		Vec2 edge2_uv = uv3 - uv1;
		
		float cp = edge1_uv.y * edge2_uv.x - edge1_uv.x * edge2_uv.y;
		if(cp != 0.0f) {
			float mul = 1.0f/cp; // flips
			Vec3 tangent = (edge1 * -edge2_uv.y + edge2 * edge1_uv.y) * mul;
			Vec3 bitangent = (edge1 * -edge2_uv.x  + edge2 * edge1_uv.x) * mul;

			
			// these tangents are not per se orthogonal but most likely are 
			// near orthogonal. we use Gram-Schhmidt orthogonalization, by 
			// subtracting the part of a vector in the direction of another
			// vector and then renormalizing. (see linke above 3dkingdoms,
			// or page: 186 in the above mentioned book.
			Vec3 normal = normals[triangles[i].a];
			tangent -= normal * normal.dot(tangent);
			bitangent -= bitangent.dot(normal)*normal - (tangent.dot(bitangent)*tangent);

			tangent.normalize();
			bitangent.normalize();
			
			tangents[triangles[i].a] = tangent;
			bitangents[triangles[i].a] = bitangent;
			tangents[triangles[i].b] = tangent;
			bitangents[triangles[i].b] = bitangent;
			tangents[triangles[i].c] = tangent;
			bitangents[triangles[i].c] = bitangent;
		}
	}
	attribs |= VERT_BINORM;
	attribs |= VERT_TAN;
	
};

/*
void VertexData::calculateTangentAndBiTangent() {
	if(triangles.size() == 0 || texcoords.size() == 0) {
		ofLog(OF_LOG_ERROR, "Error: cannot calculate (bi)tangents as we do not have any triangles, or no texcoords defined.");
		return;
	}
	
	if(tangents.size() == 0 && bitangents.size() == 0) {
		Vec3 d(0,0,0);
		tangents.assign(getNumVertices(), d);
		bitangents.assign(getNumVertices(), d);
	}
	for(int i = 0; i < triangles.size(); ++i) {
		Vec3& v1 = vertices[triangles[i].a];
		Vec3& v2 = vertices[triangles[i].b];
		Vec3& v3 = vertices[triangles[i].c];		
		Vec2& t1 = texcoords[triangles[i].a];
		Vec2& t2 = texcoords[triangles[i].b];		
		Vec2& t3 = texcoords[triangles[i].c];
		Vec3& normal = normals[triangles[i].a];
		Vec3 tangent(0);
		Vec3 bitangent(0);
		
		createTangentAndBiTangent(v1, v2, t1, t2, normal, tangent, bitangent);
		tangents[triangles[i].a] = tangent;
		bitangents[triangles[i].a] = bitangent;

		createTangentAndBiTangent(v1, v3, t1, t3, normal, tangent, bitangent);
		tangents[triangles[i].b] = tangent;
		bitangents[triangles[i].b] = bitangent;
		cout << tangent << endl;
		cout << bitangent << endl;
		cout << "-------\n";
//		tangents[triangles[i].c] = tangent;
//		bitangents[triangles[i].c] = bitangent;


	
	}
	attribs |= VERT_BINORM;
	attribs |= VERT_TAN;
	
};
*/
/**
 *
 *
 * va	= vertex pos a
 * vb 	= vertex pos b
 * ta 	= texcoord a
 * tb 	= texcoord b
 * 
 */ 
void VertexData::createTangentAndBiTangent(
	 Vec3 va
	,Vec3 vb
	,Vec2 ta
	,Vec2 tb
	,Vec3& normal
	,Vec3& out_tangent
	,Vec3& out_bitangent) 
{
	Vec3 norm = normal;
	float coef = 1.0f / (ta.x * tb.y - tb.x * ta.y);
	out_tangent.x = coef * ((va.x * tb.y) + (vb.x * -ta.y));
	out_tangent.y = coef * ((va.y * tb.y) + (vb.y * -ta.y)); 
	out_tangent.y = coef * ((va.z * tb.y) + (vb.y * -ta.y)); 
	out_bitangent = norm.getCrossed(out_tangent);
}


// clear all buffers.
void VertexData::clear() {
	normals.clear();
	vertices.clear();
	texcoords.clear();
	colors.clear();
	indices.clear();
	triangles.clear();
	quads.clear();
	tangents.clear();
	bitangents.clear();
}

