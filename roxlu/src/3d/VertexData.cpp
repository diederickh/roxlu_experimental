#include "VertexData.h"

namespace roxlu {

int VertexData::num_instances = 0;

VertexData::VertexData() 
	:vertex_p(NULL)
	,vertex_pt(NULL)
	,vertex_pn(NULL)
	,vertex_ptn(NULL)
	,vertex_ptnt(NULL)
	,vertex_pnc(NULL)
	,vertex_ptntb(NULL)
	,attribs(VERT_NONE)
{
	++num_instances;
	char auto_name[30];
	sprintf(auto_name, "VertexData%04d", num_instances);
	name = auto_name;
	printf("Autoname: %s\n", name.c_str());
}

VertexData::VertexData(const string& meshName) 
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

int VertexData::addVertex(const Vec3& vec) {
	vertices.push_back(vec);
	attribs |= VERT_POS;
	return vertices.size()-1;
}

int VertexData::addVertex(const float x, const float y, const float z) {
	vertices.push_back(Vec3(x,y,z));
	attribs |= VERT_POS;
	return vertices.size()-1;
}

vector<int> VertexData::addVertices(const vector<Vec3>& copy) {
	vector<int> new_indices;
	vector<Vec3>::const_iterator it = copy.begin();
	while(it != copy.end()) {
		new_indices.push_back(addVertex(*it));
		++it;
	}	
	attribs |= VERT_POS;
	return new_indices;
}

int VertexData::addTexCoord(const Vec2& vec) {
	texcoords.push_back(vec);
	attribs |= VERT_TEX;
	return texcoords.size()-1;
}

int VertexData::addTexCoord(const float x, const float y) {
	texcoords.push_back(Vec2(x, y));
	attribs |= VERT_TEX;
	return texcoords.size()-1;
}

int VertexData::addColor(const Vec3& color) {
	return addColor(Color4(color.x, color.y, color.z, 1));
}

int VertexData::addColor(const float r, const float g, const float b) {
	return addColor(Color4(r, g, b, 1.0f));
}

int VertexData::addColor(const Color4& color) {
	colors.push_back(color);
	attribs |= VERT_COL;
	return colors.size()-1;
}

int VertexData::addNormal(const float x, const float y, const float z) {
	normals.push_back(Vec3(x, y, z));
	attribs |= VERT_NORM;
	return normals.size()-1;
}

int VertexData::addNormal(const Vec3& vec) {
	normals.push_back(vec);
	attribs |= VERT_NORM;
	return normals.size()-1;
}

void VertexData::addIndex(const int& index) {
	indices.push_back(index);
}

int	VertexData::addTangent(const Vec3& tangent) {
	tangents.push_back(tangent);
	return tangents.size()-1;
}

int	VertexData::addBinormal(const Vec3& binorm) {
	binormals.push_back(binorm);
	return binormals.size()-1;
}


// triangles are i.e. used when exporting. It's just a simple reference
int VertexData::addTriangle(const int a, const int b, const int c) {
	Triangle t(a,b,c);
	triangles.push_back(t);
	return triangles.size()-1;
}

int	VertexData::addTriangle(Triangle t) {
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

int VertexData::addQuadAndIndices(int a, int b, int c, int d) {
	addQuadIndices(a,b,c,d);
	return addQuad(a,b,c,d);
}

void VertexData::addQuadIndices(int a, int b, int c, int d) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	indices.push_back(d);
}

int VertexData::addQuad(int a, int b, int c, int d) {
	Quad q(a, b, c, d);
	quads.push_back(q);
	return quads.size()-1;
}

int VertexData::addQuad(Quad q) {
	quads.push_back(q);
	return quads.size()-1;	
}

Triangle& VertexData::getTriangle(int index) {
	return triangles[index];
}

Triangle* VertexData::getTrianglePtr(int triangle) {
	return &triangles[triangle];	
}

Quad* VertexData::getQuadPtr(int quad) {
	return &quads[quad];
}


Vec3 VertexData::getVertex(int index) {
	return vertices[index];
}

Vec3* VertexData::getVertexPtr(int index) {
	return &vertices[index];
}

Vec3& VertexData::getVertexRef(int index) {
	return vertices[index];
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

Vec3& VertexData::getNormalRef(int index) {
	return normals[index];
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
		vertex_ptntb[i].binorm = binormals[i]; 
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
/*
	vector<Vec3> tan1;
	vector<Vec3> tan2;
	int len = vertices.size();
	tan1.assign(len, Vec3());
	tan2.assign(len, Vec3());
	if(quads.size() > 0) {
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
	// this implementation isn't working yet
	if(triangles.size() > 0) {
		
//		tan1.assign(len, Vec3());
//		tan2.assign(len, Vec3());
		for(int i = 0; i < triangles.size(); ++i) {
			Triangle& t = triangles[i];
			Vec3 sdir;
			Vec3 tdir;
			
			// first triangle.
			computeTangentForTriangle(
				vertices[t.va]
				,vertices[t.vb]
				,vertices[t.vc]
				,texcoords[t.va]
				,texcoords[t.vb]
				,texcoords[t.vc]
				,sdir
				,tdir
			);
			
			tan1[t.va] += sdir;
			tan1[t.vb] += sdir;
			tan1[t.vc] += sdir;
			
			tan2[t.va] += tdir;
			tan2[t.vb] += tdir;
			tan2[t.vc] += tdir;
		}
	}
	

	for(int i = 0; i < len; ++i) {
		Vec3& n = normals[i];
		Vec3& t = tan1[i];
		Vec4 tangent = (t - n * (n.dot(t))).normalize();
		tangent.w = (tan2[i].dot(n.getCrossed(t)) < 0.0f) ? -1.0f : 1.0f;
		tangents.push_back(tangent);
	}
	
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
	/*
	Vec3 norm = normal;
	float coef = 1.0f / (ta.x * tb.y - tb.x * ta.y);
	out_tangent.x = coef * ((va.x * tb.y) + (vb.x * -ta.y));
	out_tangent.y = coef * ((va.y * tb.y) + (vb.y * -ta.y)); 
	out_tangent.y = coef * ((va.z * tb.y) + (vb.y * -ta.y)); 
	out_bitangent = norm.getCrossed(out_tangent);
	*/
}


void VertexData::debugDraw(int drawMode) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
	// we adjust the size of the normals lines dynamically
	float line_length = 0.2;
	if(vertices.size() > 2) {
		float length = (vertices[1] - vertices[2]).length();
		if(length >= 3) {
			line_length = 3.2;
		}
	}


	float colors[4][3] = {
			{0.88,0.25,0.11}
			,{0.6,0.78,0.212}
			,{0.2,0.65,0.698}
			,{0.94,0.72,0.29}
	};
	 
	// draw using indices
	if(getNumTriangles() > 0) {
		
		// triangles (fills)
		Triangle tri;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.8,0.8,0.8);
		glBegin(GL_TRIANGLES);
			for(int i = 0; i < triangles.size(); ++i) {	
				tri = triangles[i];
				glVertex3fv(vertices[tri.va].getPtr());
				glVertex3fv(vertices[tri.vb].getPtr());
				glVertex3fv(vertices[tri.vc].getPtr());
			}
		glEnd();
		
		// triangles lines.
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.1,0.1,0.1);
		glBegin(GL_TRIANGLES);
			for(int i = 0; i < triangles.size(); ++i) {	
				tri = triangles[i];
				glVertex3fv(vertices[tri.va].getPtr());
				glVertex3fv(vertices[tri.vb].getPtr());
				glVertex3fv(vertices[tri.vc].getPtr());
			}
		glEnd();
		
		// normals, tangents and binormals
		bool draw_normals= true;
		bool draw_tangents = true;
		bool draw_binormals = true;
		float s = 0.2;
		glBegin(GL_LINES);
			for(int i = 0; i < triangles.size(); ++i) {	
				tri = triangles[i];
				
				// normal - a
				if(draw_normals && tri.na) {
					glColor3f(0,1,1);
					glVertex3fv(vertices[tri.va].getPtr());
					glVertex3fv((vertices[tri.va]+normals[tri.na]*s).getPtr());	
				}

				// tangent - a
				if(draw_tangents && tri.ta) {
					glColor3f(1,0,0);
					glVertex3fv(vertices[tri.va].getPtr());
					glVertex3fv((vertices[tri.va]+(tangents[tri.ta]*s)).getPtr());	
				}

				// binormal - a 
				if(draw_binormals && tri.ba) {
					glColor3f(0,1,0);
					glVertex3fv(vertices[tri.va].getPtr());
					glVertex3fv((vertices[tri.va]+(binormals[tri.ba]*s)).getPtr());	
				}
				
				// normal - b
				if(draw_normals && tri.nb) {
					glColor3f(0,1,1);
					glVertex3fv(vertices[tri.vb].getPtr());
					glVertex3fv((vertices[tri.vb]+normals[tri.nb]*s).getPtr());	
				}
				
				// tangent - b
				if(draw_tangents && tri.tb) {
					glColor3f(1,0,0);
					glVertex3fv(vertices[tri.vb].getPtr());
					glVertex3fv((vertices[tri.vb]+(tangents[tri.tb]*s)).getPtr());	
				}
				
				// binormal - b
				if(draw_binormals && tri.bb) {
					glColor3f(0,1,0);
					glVertex3fv(vertices[tri.vb].getPtr());
					glVertex3fv((vertices[tri.vb]+(binormals[tri.bb]*s)).getPtr());	
				}
				
				// normal - c
				if(draw_normals && tri.nc) {
					glColor3f(0,1,1);
					glVertex3fv(vertices[tri.vc].getPtr());
					glVertex3fv((vertices[tri.vc]+normals[tri.nc]*s).getPtr());	
				}
				
				// tangent - c
				if(draw_tangents && tri.tc) {
					glColor3f(1,0,0);
					glVertex3fv(vertices[tri.vc].getPtr());
					glVertex3fv((vertices[tri.vc]+(tangents[tri.tc]*s)).getPtr());	
				}
				
				// binormal - c
				if(draw_binormals && tri.bc) {
					glColor3f(0,1,0);
					glVertex3fv(vertices[tri.vc].getPtr());
					glVertex3fv((vertices[tri.vc]+(binormals[tri.bc]*s)).getPtr());	
				}

			}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if(getNumIndices() > 0) {
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
			
			Vec4 colors[4];
			colors[0].set(1,0,0, 0.8);
			colors[1].set(0,1,0, 0.8);
			colors[2].set(0,0,1, 0.8);
			colors[3].set(1,1,0, 0.8);
			
			glBegin(drawMode);
			for(int i = 0; i < len; ++i) {
				glColor4fv(colors[i%mod].getPtr());
				glVertex3fv(vertices[indices[i]].getPtr());
			}
			glEnd();
			
			if(normals.size() > 0) {
				glBegin(GL_LINES);
				for(int i = 0; i < len; ++i) {
					int dx = indices[i];
					//printf("%d <--\n", dx);
					Vec3 pos = vertices[dx];
					Vec3 norm = normals[dx];
					
					Vec3 end = pos + (norm.scale(line_length*2));
					glColor4f(1.0f,0.0f,0.4f,1.0f);
					glColor4f(0.98, 0.92, 0.75, 0.6);
					glVertex3fv(pos.getPtr());
					glColor4f(1.0f, 0.0f,1.0f,1.0f);
					glColor3f(0.98, 0.92, 0.75);
					glColor4f(0.98, 0.92, 0.75,0.6);
					glVertex3fv(end.getPtr());
					
				}
				glEnd();
			}


		}
	}
	// w/o indices
	else {

		glColor4f(1,1,1,1);
		glUseProgram(0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		bool draw_texture = false;		
		bool draw_tangents = true;
		bool draw_normals = true;
		bool draw_binormals = true;
		if(draw_texture && getNumTexCoords() > 0) {
			glColor3f(0.98, 0.92, 0.75); // yellowish
			int len = vertices.size();
			glEnable(GL_TEXTURE_2D);
			glBegin(drawMode);
			for(int i = 0; i < len; ++i) {
				int parts = 3;
				if(drawMode == GL_QUADS) {
					parts = 4;
				}
				glColor3fv(colors[i%parts]);
				glTexCoord2fv(texcoords[i].getPtr());
				glVertex3fv(vertices[i].getPtr());
			}
			glEnd();
			glDisable(GL_TEXTURE_2D);
			
		
		}
		else {
			// shape	
			//glColor3f(0.98, 0.92, 0.75); // yellowish
			glColor3f(0.8,0.8,0.8);
			glBegin(drawMode);
			vector<Vec3>::iterator it = vertices.begin();
			float len = vertices.size();
			while(it != vertices.end()) {	
				glVertex3fv((*it).getPtr());
				++it;
			}
			glEnd();
		}

		// normals.
		int len = normals.size();
		if(draw_normals && len == vertices.size()) {	
			glColor3f(0,1,1);
			glLineWidth(3.0);
			glBegin(GL_LINES);
			for(int i = 0; i < len; ++i) {
				Vec3 pos = vertices[i];
				Vec3 norm = normals[i];
				norm.normalize();
				Vec3 end = pos + (norm.scale(line_length));
				glVertex3fv(pos.getPtr());
				glVertex3fv(end.getPtr());
			}
			glEnd();
		}
		
		// tangents
		if(draw_tangents && tangents.size() > 0) {
			glLineWidth(3.0);
			glColor3f(1.0f,0.0f,0.0f);
			int len = tangents.size();
			glBegin(GL_LINES);
			for(int i = 0; i < len; ++i) {
				Vec3 pos = vertices[i];
				Vec3 norm = tangents[i];
				norm.normalize();
				Vec3 end = pos + (norm.scale(line_length));
				glVertex3fv(pos.getPtr());
				glVertex3fv(end.getPtr());
			};
			glEnd();
		}

		// binormals
		if(draw_binormals && binormals.size() > 0) {
			glLineWidth(3.0);
			glColor4f(0.0f,1.0f,0.0f,1.0f);
			int len = tangents.size();
			glBegin(GL_LINES);
			for(int i = 0; i < len; ++i) {
				Vec3 pos = vertices[i];
				Vec3 binorm = binormals[i].normalize();
				Vec3 end = pos + (binorm.scale(line_length));
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
		glLineWidth(0.5f);
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.4,0.4,0.4);
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
		glColor3f(1,1,1);

		
	}
	
//	if(!blend_enabled) {
//		glDisable(GL_BLEND);
//	}
	
	glPopAttrib();
	
}


void VertexData::debugDrawQuad(int quadNum) {
	Quad* q = getQuadPtr(quadNum);
	if(q == NULL) {
		printf("Quad not found: %d\n", quadNum);
		return;
	}
	glBegin(GL_QUADS);
		glVertex3fv(vertices[q->a].getPtr());
		glVertex3fv(vertices[q->b].getPtr());
		glVertex3fv(vertices[q->c].getPtr());
		glVertex3fv(vertices[q->d].getPtr());
	glEnd();
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
	binormals.clear();
}

Vec3 VertexData::computeQuadNormal(int quad) {
	Quad& q = quads[quad];
	Vec3 ab = vertices[q.b] - vertices[q.a];
	Vec3 bc = vertices[q.c] - vertices[q.b];
	Vec3 crossed = ab.getCrossed(bc);
	return crossed;
}

// Debug
//------------------------------------------------------------------------------
void VertexData::print() {
	printf("\nVertices:\n"); printVertices();
	printf("\nNormals:\n"); printNormals();
	printf("\nTangents:\n");printTangents();
	printf("\nBinormals:\n"); printBinormals();
	printf("\nTexCoords:\n"); printTexCoords();
	printf("\nTriangles:\n"); printTriangles();	
}

void VertexData::printVertices() {
	for(int i = 0; i < vertices.size(); ++i) {
		vertices[i].print();
	}
}

void VertexData::printNormals() {
	for(int i = 0; i < normals.size(); ++i) {
		normals[i].print();
	}
}

void VertexData::printTangents() {
	for(int i = 0; i < tangents.size(); ++i) {
		tangents[i].print();
	}
}

void VertexData::printBinormals() {
	for(int i = 0; i < binormals.size(); ++i) {
		binormals[i].print();
	}
}

void VertexData::printTexCoords() {
	for(int i = 0; i < texcoords.size(); ++i) {
		texcoords[i].print();
	}
}

void VertexData::printTriangles() {	
	for(int i = 0; i < triangles.size(); ++i) {
		Triangle& tri = triangles[i];
		printf("va/vb/vc: (%d, %d, %d), na/nb/nc: (%d, %d, %d), ta/tb/tc: (%d, %d, %d), ba/bb/bc: (%d, %d, %d), tc_a/tc_b,tc_c: (%d, %d, %d)\n"
			,tri.va, tri.vb, tri.vc
			,tri.na, tri.nb, tri.nc
			,tri.ta, tri.tb, tri.tc
			,tri.ba, tri.bb, tri.bc
			,tri.tc_a, tri.tc_b, tri.tc_c
		);
	}
}

} // roxlu
