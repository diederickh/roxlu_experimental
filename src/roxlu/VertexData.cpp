#include "VertexData.h"
namespace roxlu {

VertexData::VertexData(string meshName) 
:vertex_p(NULL)
,vertex_pt(NULL)
,vertex_pn(NULL)
,vertex_ptn(NULL)
,vertex_ptnt(NULL)
,vertex_pnc(NULL)
,vertex_ptntb(NULL)
,attribs(VERT_NONE)
,name(meshName)
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
	addColor(Color4(rColor.x, rColor.y, rColor.z, 1));
}

void VertexData::addColor(const float nR, const float nG, const float nB) {
	addColor(Color4(nR, nG, nB, 1.0f));
}

void VertexData::addColor(const Color4& rCol) {
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

// triangles are i.e. used when exporting. It's just a simple reference
int VertexData::addTriangle(const int a, const int b, const int c) {
	Triangle t(a,b,c);
	triangles.push_back(t);
	return triangles.size()-1;
}

int VertexData::addTriangleAndIndices(const int a, const int b, const int c) {
	// keep track of triangle.
	Triangle t(a, b, c);
	triangles.push_back(t);
	
	// also create indexed triangle.
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	
	// return triangle index.
	return triangles.size()-1;
}

int VertexData::addQuad(int nA, int nB, int nC, int nD) {
	Quad q(nA, nB, nC, nD);
	quads.push_back(q);
	return quads.size()-1;
}

int VertexData::addQuad(Quad q) {
	quads.push_back(q);
	return quads.size()-1;	
}

Triangle* VertexData::getTrianglePtr(int nTriangle) {
	return &triangles[nTriangle];	
}

Quad* VertexData::getQuadPtr(int nQuad) {
	return &quads[nQuad];
}


Vec3 VertexData::getVertex(int index) {
	return vertices[index];
}

Vec3* VertexData::getVertexPtr(int index) {
	return &vertices[index];
}

Vec2 VertexData::getTexCoord(int index) {
	return texcoords[index];
}

Vec2* VertexData::getTexCoordPtr(int index) {
	return &texcoords[index];
}

Vec3 VertexData::getNormal(int index) {
	return normals[index];
}

Vec3* VertexData::getNormalPtr(int index) {
	return &normals[index];
}

const float* VertexData::getVerticesPtr() {
	return &vertices[0].x;
}

const float* VertexData::getTexCoordsPtr() {
	return &texcoords[0].x;
}

const float* VertexData::getNormalsPtr() {
	return &normals[0].x;
}

const float* VertexData::getColorsPtr() {
	return &colors[0].r;
}

const int* VertexData::getIndicesPtr() {
	return &indices[0];
}

int VertexData::getNumVertices() {
	return (int)vertices.size();
}

int VertexData::getNumTexCoords() {
	return (int)texcoords.size();
}

int VertexData::getNumTangents() {
	return (int)tangents.size();
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

VertexPT* VertexData::getVertexPT() {
	if(vertex_pt != NULL) {
		return vertex_pt;
	}
	if( ! (attribs & (VERT_POS | VERT_TEX)) ) {
		return NULL;
	}
	int num = getNumVertices();
	vertex_pt = new VertexPT[num];
	for(int i = 0; i < num; ++i) {	
		vertex_pt[i].pos = vertices[i]; 
		vertex_pt[i].tex = texcoords[i]; 
	}
	return vertex_pt;
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

VertexPTNT* VertexData::getVertexPTNT() {
	if(vertex_ptnt != NULL) { 	
		return vertex_ptnt; 
	}
	if( ! (attribs & (VERT_POS | VERT_NORM | VERT_TEX | VERT_TAN)) ) { 	
		return NULL;  
	}

	if(tangents.size() == 0) {
		computeTangents();
	}	
	
	int num = getNumVertices();
	vertex_ptnt = new VertexPTNT[num];
	for(int i = 0; i < num; ++i) {	
		vertex_ptnt[i].pos = vertices[i]; 
		vertex_ptnt[i].norm = normals[i]; 
		vertex_ptnt[i].tex = texcoords[i]; 
		vertex_ptnt[i].tan = tangents[i]; 
	}
	return vertex_ptnt;
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
 * - 	Based on:
 * 		http://www.terathon.com/code/tangent.html
 *
 * TODO: this method should be optimized as it loops over all triangles so
 * doing things twice or more for shared vertices.
 */
 
void VertexData::computeTangentForTriangle(Vec3& v1, Vec3& v2, Vec3& v3, Vec2& w1, Vec2& w2, Vec2& w3, Vec3& sdir, Vec3& tdir) {
	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;
	
	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;
	
	float r = 1.0f / (s1 * t2 - s2 * t1);
	sdir.set(
		 (t2 * x1 - t1 * x2) * r
		,(t2 * y1 - t1 * y2) * r
		,(t2 * z1 - t1 * z2) * r
	);
	
	tdir.set(
		 (s1 * x2 - s2 * x1) * r
		,(s1 * y2 - s2 * y1) * r
		,(s1 * z2 - s2 * z1) * r
	);
}
 
void VertexData::computeTangents() {
	vector<Vec3> tan1;
	vector<Vec3> tan2;
	int len = vertices.size();
	if(quads.size() > 0) {
		tan1.assign(len, Vec3());
		tan2.assign(len, Vec3());
		for(int i = 0; i < quads.size(); ++i) {
			Quad& q = quads[i];
			Vec3 sdir;
			Vec3 tdir;
			
			// first triangle.
			computeTangentForTriangle(
				vertices[q.a]
				,vertices[q.b]
				,vertices[q.c]
				,texcoords[q.a]
				,texcoords[q.b]
				,texcoords[q.c]
				,sdir
				,tdir
			);
			
			tan1[q.a] += sdir;
			tan1[q.b] += sdir;
			tan1[q.c] += sdir;
			
			tan2[q.a] += tdir;
			tan2[q.b] += tdir;
			tan2[q.c] += tdir;
			
			// second triangle
			computeTangentForTriangle(
				vertices[q.c]
				,vertices[q.d]
				,vertices[q.a]
				,texcoords[q.c]
				,texcoords[q.d]
				,texcoords[q.a]
				,sdir
				,tdir
			);
			
			tan1[q.c] += sdir;
			tan1[q.d] += sdir;
			tan1[q.a] += sdir;
			
			tan2[q.c] += tdir;
			tan2[q.d] += tdir;
			tan2[q.a] += tdir;
		
		}
	}
	
	// @todo implement for just triangles!
	
	for(int i = 0; i < len; ++i) {
		Vec3& n = normals[i];
		Vec3& t = tan1[i];
		Vec4 tangent = (t - n * (n.dot(t))).normalize();
		tangent.w = (tan2[i].dot(n.getCrossed(t)) < 0.0f) ? -1.0f : 1.0f;
		tangents.push_back(tangent);
	}
	
	attribs |= VERT_TAN;
	return;
	/*
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
	*/
};
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


void VertexData::debugDraw(int drawMode) {
	float colors[4][3] = {
			{0.88,0.25,0.11}
			,{0.6,0.78,0.212}
			,{0.2,0.65,0.698}
			,{0.94,0.72,0.29}
	};
	 
	// draw using indices
	if(getNumIndices() > 0) {
		if(getNumTexCoords() > 0) {
			int len = indices.size();
			glBegin(drawMode);
			for(int i = 0; i < len; ++i) {
				glTexCoord2fv(texcoords[indices[i]].getPtr());
				glVertex3fv(vertices[indices[i]].getPtr());
			}
			glEnd();
		}
		else {
			int len = indices.size();
			int mod = (drawMode == GL_QUADS) ? 4 : 3;
			Vec3 colors[4];
			colors[0].set(1,0,0);
			colors[1].set(0,1,0);
			colors[2].set(0,0,1);
			colors[3].set(1,1,0);
			
			glBegin(drawMode);
			for(int i = 0; i < len; ++i) {
				glColor3fv(colors[i%mod].getPtr());
				glVertex3fv(vertices[indices[i]].getPtr());
			}
			glEnd();
		}
	}
	// w/o indices
	else {
	
		glUseProgram(0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		// we adjust the size of the normals lines dynamically
		float line_length = 0.2;
		if(vertices.size() > 2) {
			float length = (vertices[1] - vertices[2]).length();
			if(length >= 3) {
				line_length = 3.2;
			}
		}
		
		if(getNumTexCoords() > 0) {
			glColor3f(0.98, 0.92, 0.75); // yellowish
			int len = vertices.size();
			glBegin(drawMode);
			for(int i = 0; i < len; ++i) {
				int parts = 3;
				if(drawMode == GL_QUADS) {
					parts = 4;
				}
				 glColor3fv(colors[i%parts]);
				//glTexCoord2fv(texcoords[i].getPtr());
				glVertex3fv(vertices[i].getPtr());
			}
			glEnd();
			//glEnable(GL_TEXTURE_2D);
		
		}
		else {
			// shape	
			glColor3f(0.98, 0.92, 0.75); // yellowish
			glBegin(drawMode);
			vector<Vec3>::iterator it = vertices.begin();
			float len = vertices.size();
			int i = 0;
			while(it != vertices.end()) {	
				int parts = 3;
				if(drawMode == GL_QUADS) {
					parts = 4;
				}
				glColor3fv(colors[i%parts]);
				glVertex3fv((*it).getPtr());
				++it;
				++i;
			}

			glEnd();
		}

		// normals.
		int len = normals.size();
		if(len == vertices.size()) {	
			glColor3f(1,1,1);
			glLineWidth(1.5);
			
			glDisable(GL_BLEND);
			glBegin(GL_LINES);

			for(int i = 0; i < len; ++i) {
				Vec3 pos = vertices[i];
				Vec3 norm = normals[i];
				norm.normalize();
				Vec3 end = pos + (norm.scale(line_length));
				glColor4f(1.0f,0.0f,0.4f,1.0f);
				glColor3f(0.98, 0.92, 0.75);
				glVertex3fv(pos.getPtr());
				glColor4f(1.0f, 0.0f,1.0f,1.0f);
				glColor3f(0.98, 0.92, 0.75);
				glVertex3fv(end.getPtr());
			}
			glEnd();
		}
		
		// tangents
		if(tangents.size() > 0) {
			int len = tangents.size();
			glBegin(GL_LINES);
			for(int i = 0; i < len; ++i) {
				Vec3 pos = vertices[i];
				Vec3 norm = tangents[i];
				norm.normalize();
				Vec3 end = pos + (norm.scale(line_length));
				glColor4f(1.0f,0.0f,0.0f,1.0f);
				glVertex3fv(pos.getPtr());
				glVertex3fv(end.getPtr());
			};
			glEnd();
		}
		
		// draw lines
		glPolygonOffset(-1.0f, -1.0f);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(2.5f);
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0,0,0);
		glBegin(drawMode);
		for(int i = 0; i < vertices.size(); ++i) {
			glVertex3fv(vertices[i].getPtr());
		}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glLineWidth(1.0f);
		glDisable(GL_BLEND);
		glDisable(GL_LINE_SMOOTH);

		
	}
}

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

} // roxlu
