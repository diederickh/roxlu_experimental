#ifndef ROXLU_MIKKELSENH
#define ROXLU_MIKKELSENH


#define MIKK_COPY_VEC2(d,s) d[0] = s.x; d[1] = s.y;
#define MIKK_COPY_VEC3(d,s) d[0] = s.x; d[1] = s.y; d[2] = s.z;
#define ROXLU_COPY_MIKK3(d,s) d.x = s[0]; d.y = s[1]; d.z = s[2];

#include "VertexData.h"
#include <vector>

extern "C" {
	#include "mikktspace.h"
}

using std::vector;
using namespace roxlu;

class Mikkelsen {
public:
	void createTangentSpaces(VertexData& vd, VertexData& result);
//	VertexData* vd;
	VertexData* input;
	VertexData* result;
	vector<float> tangent_signs;	
	
	static int getNumFaces(const SMikkTSpaceContext* context);
	static int getNumVerticesOfFace(const SMikkTSpaceContext* context, const int faceNum);
	static void getPosition(const SMikkTSpaceContext* context, float pos[], const int faceNum, const int vertexIndex);
	static void getTexCoord(const SMikkTSpaceContext* context, float uv[], const int faceNum, const int vertexIndex);
	static void getNormal(const SMikkTSpaceContext* context, float norm[], const int faceNum, const int vertexIndex);
	static void setTSpaceBasic(const SMikkTSpaceContext* context, const float tangent[], const float sign, const int faceNum, const int vertexIndex);
};

#endif