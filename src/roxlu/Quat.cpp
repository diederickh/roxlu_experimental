#include "Quat.h"

namespace roxlu {

Quat::Quat()
:w(1)
{
}

void Quat::toMat4(Mat4& rDest) const {
	float tx  = 2.0f*v.x;
	float ty  = 2.0f*v.y;
	float tz  = 2.0f*v.z;
	float twx = tx*w;
	float twy = ty*w;
	float twz = tz*w;
	float txx = tx*v.x;
	float txy = ty*v.x;
	float txz = tz*v.x;
	float tyy = ty*v.y;
	float tyz = tz*v.y;
	float tzz = tz*v.z;
	rDest.m[0] = 1.0f-(tyy+tzz);
	rDest.m[1] = txy-twz;
	rDest.m[2] = txz+twy;
	
	rDest.m[4] = txy+twz;
	rDest.m[5] = 1.0f-(txx+tzz);
	rDest.m[6] = tyz-twx;
	
	rDest.m[8] = txz-twy;
	rDest.m[9] = tyz+twx;
	rDest.m[10] = 1.0f-(txx+tyy);
	//rDest.flags |= rDest.ROTATE;
}

Mat4 Quat::getMat4() const {
	Mat4 result;
	toMat4(result);
	return result;
}

// angle in radians!
void Quat::rotate(float nAngle, const float nX, const float nY, const float nZ) {
	Quat q;
	float ha = 0.5 * nAngle;
	float s = sin(ha);
	q.w = cos(ha);
	q.v.x = s * nX;
	q.v.y = s * nY;
	q.v.z = s * nZ;
	Quat cp = *this;
	cp = cp * q;
	*this = cp;
}

void Quat::fromAngleAxis(float nAngle, float nX, float nY, float nZ) {
	float ha = 0.5 * nAngle;
	float s = sin(ha);
	w = cos(ha);
	v.x = s * nX;
	v.y = s * nY;
	v.z = s * nZ;
}

Vec3 Quat::rotate(const Vec3& rVec) const {
	float vmult = (2.0f * v.x * rVec.x + v.y * rVec.y + v.z * rVec.z);
	float cross_mult = 2.0f * w;
	float pmult = cross_mult*w - 1.0f;
	return Vec3(
		pmult * rVec.x + vmult * v.x + cross_mult * (v.y * rVec.z - v.z * rVec.y)
		,pmult * rVec.y + vmult * v.y + cross_mult * (v.z * rVec.x - v.x * rVec.z)
		,pmult * rVec.z + vmult * v.z + cross_mult * (v.x * rVec.y - v.y * rVec.x)
	);
}

void Quat::normalize() {
	float len = w*w + v.x*v.x + v.y*v.y + v.z*v.z;
	float factor = 1.0f / sqrt(len);
	v = v * factor;
	w = w * factor;
}

} // roxlu
