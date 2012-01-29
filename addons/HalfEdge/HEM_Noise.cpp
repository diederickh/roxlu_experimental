#include "HEM_Noise.h"
#include "HE_Mesh.h"
#include "HE_Headers.h"
//#include "simplexnoise1234.h"

namespace roxlu {

HEM_Noise::HEM_Noise()
	:x_scale(0.05)
	,y_scale(0.006)
	,z_scale(0.2)
	,displacement(1.4)
{
}

bool HEM_Noise::modify(HE_Mesh& m) {
	const vector<HE_Vertex*>& verts = m.getVerticesRef();
	vector<HE_Vertex*>::const_iterator it = verts.begin();
	float c = 0;
	while(it != verts.end()) {
		HE_Vertex& v = *(*it);
		Vec3& pos = v.getPositionRef();
		
		printf("@todo not yet included simplexnoise in roxlu lib\n");
//		pos.x += (snoise3(pos.x * x_scale, pos.y * x_scale, pos.z * x_scale)) * displacement;
//		pos.y += (snoise3(pos.x * y_scale, pos.y * y_scale, pos.z * y_scale)) * displacement;
//		pos.z += (snoise3(pos.x * z_scale, pos.y * z_scale, pos.z * z_scale)) * displacement;
		++it;
	}
}

}; // roxlu