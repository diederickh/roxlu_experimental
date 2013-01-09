//#include "Vec2.h"
//#include "Vec3.h"


#include <roxlu/math/Vec2.h>
#include <roxlu/math/Vec3.h>

namespace roxlu {

Vec2::Vec2()
	:x(0.0f)
	,y(0.0f)
{
}

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


}; // roxlu
