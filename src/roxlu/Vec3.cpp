#include "Vec3.h"
#include "Vec4.h"
namespace roxlu {

Vec3::Vec3(const Vec4& v4) 
	:x(v4.x * v4.w)
	,y(v4.y * v4.w)
	,z(v4.z * v4.w)
{
	
}

Vec3& Vec3::operator=(const Vec4& v4) {
	x = v4.x;
	y = v4.y;
	z = v4.z;
	return *this;
}

}; // roxlu


