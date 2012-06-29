#include "Mikkelsen.h"

/*
Triangle struct members:
-------------------
	int va, vb, vc;	 // vertex indices
	int na, nb, nc;  // normal indices
	int ta, tb, tc;  // tangent indices
	int ba, bb, bc;  // binormal indices
	int tc_a, tc_b, tc_c; // texcoord indices
*/

void Mikkelsen::createTangentSpaces(VertexData& data, VertexData& out) {
	
	// Copy input (data) into the output buffer 
	input = &data;
	result = &out;
	for(int i = 0; i < data.triangles.size(); ++i) {
		Triangle& tri = data.triangles[i];
		
		// unindex the vertices.
		result->addVertex(data.vertices[tri.va]);
		result->addVertex(data.vertices[tri.vb]);
		result->addVertex(data.vertices[tri.vc]);
		
		// unindex the texcoords
		result->addTexCoord(data.texcoords[tri.tc_a]);
		result->addTexCoord(data.texcoords[tri.tc_b]);
		result->addTexCoord(data.texcoords[tri.tc_c]);
		
		// unindex the normals
		result->addNormal(data.normals[tri.na]);
		result->addNormal(data.normals[tri.nb]);
		result->addNormal(data.normals[tri.nc]);
	}

	// alloc memory for tangents and bitangents, and the signs
	result->tangents.resize(result->getNumVertices());
	result->binormals.resize(result->getNumVertices());
	tangent_signs.resize(result->getNumVertices());

	// base mikkelsen structs.
	SMikkTSpaceContext mikk_context = {0};
	SMikkTSpaceInterface mikk_interface = {0};
	mikk_context.m_pUserData = this;
	mikk_context.m_pInterface = &mikk_interface;

	// setup the interface.	
	mikk_interface.m_getNumFaces = &Mikkelsen::getNumFaces;
	mikk_interface.m_getNumVerticesOfFace = &Mikkelsen::getNumVerticesOfFace;
	mikk_interface.m_getPosition = &Mikkelsen::getPosition;
	mikk_interface.m_getTexCoord = &Mikkelsen::getTexCoord;
	mikk_interface.m_getNormal = &Mikkelsen::getNormal;
	mikk_interface.m_setTSpaceBasic = &Mikkelsen::setTSpaceBasic;
	
	// this kicks off the mikkelsen functions!
	genTangSpaceDefault(&mikk_context);
	
	// after calculating all tangents, we calc the bitangents.
	// we follow this description: http://wiki.blender.org/index.php/Dev:Shading/Tangent_Space_Normal_Maps
	for(int i = 0; i < result->tangents.size(); ++i) {
		float sign = tangent_signs[i];
		result->binormals[i] = sign * cross(result->normals[i], result->tangents[i]);	
	}

}


// SMikkTSpaceInterface implementation.
// -----------------------------------------------------------------------------
int Mikkelsen::getNumFaces(
					const SMikkTSpaceContext* context
)
{
	Mikkelsen* m = static_cast<Mikkelsen*>(context->m_pUserData);
	return m->input->getNumTriangles();
}

int Mikkelsen::getNumVerticesOfFace(
					 const SMikkTSpaceContext* context
					,const int faceNum
)
{	
	return 3; // we only support triangulated shapes for now.

}
void Mikkelsen::getPosition(
					 const SMikkTSpaceContext* context
					,float pos[]
					,const int faceNum
					,const int vertexIndex
)
{

	Mikkelsen* m = static_cast<Mikkelsen*>(context->m_pUserData);
	int dx = faceNum * 3 + vertexIndex;
	MIKK_COPY_VEC3(pos, m->result->vertices[dx]);
}

void Mikkelsen::getTexCoord(
					 const SMikkTSpaceContext* context
					,float uv[]
					,const int faceNum
					,const int vertexIndex
)
{
	Mikkelsen* m = static_cast<Mikkelsen*>(context->m_pUserData);
	int dx = faceNum * 3 + vertexIndex;
	MIKK_COPY_VEC2(uv, m->result->texcoords[dx]);
}

void Mikkelsen::getNormal(
					 const SMikkTSpaceContext* context
					,float norm[]
					,const int faceNum
					,const int vertexIndex
)
{
	Mikkelsen* m = static_cast<Mikkelsen*>(context->m_pUserData);
	int dx = faceNum * 3 + vertexIndex;
	MIKK_COPY_VEC3(norm, m->result->normals[dx]);
}

void Mikkelsen::setTSpaceBasic(
					 const SMikkTSpaceContext* context
					,const float tangent[]
					,const float sign
					,const int faceNum
					,const int vertexIndex
)
{
	
	Mikkelsen* m = static_cast<Mikkelsen*>(context->m_pUserData);
	int dx = faceNum * 3 + vertexIndex;
	ROXLU_COPY_MIKK3(m->result->tangents[dx], tangent);
	m->tangent_signs[dx] = sign;
}