#include "Vec4.h"

namespace roxlu {

Vec4& Vec4::operator=(const Vec3& v3) {
	x = v3.x;
	y = v3.y;
	z = v3.z;
	return *this;
};

} // roxlu