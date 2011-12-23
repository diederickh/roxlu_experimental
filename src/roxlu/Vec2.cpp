#include "Vec2.h"
#include "Vec3.h"

namespace roxlu {

Vec2::Vec2(float xx, float yy)
	:x(xx)
	,y(yy)
{
}

Vec2::Vec2(float n) 
	:x(n)
	,y(n)
{
}

Vec2::Vec2(const Vec2& v) 
	:x(v.x)
	,y(v.y)
{
}

Vec2::Vec2(const Vec3& v) 
	:x(v.x/v.z)
	,y(v.x/v.z)
{
}

Vec2::Vec2(const Vec3& v, int dropAxis) {
	switch(dropAxis) {
		case 0: x = v.y; y = v.z; break;
		case 1: y = v.x; y = v.z; break;
		default: x = v.x; y = v.y; break;
	}
}

}; // roxlu
