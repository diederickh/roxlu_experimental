#include "HEC_FromTriangles.h"
#include "HE_Mesh.h"
#include "HE_Headers.h"

namespace roxlu {

HEC_FromTriangles::HEC_FromTriangles()
	:HEC_Creator()
{
}


bool HEC_FromTriangles::create(HE_Mesh& mesh) {
	return from_face_list.create(mesh);
}

}; // roxlu