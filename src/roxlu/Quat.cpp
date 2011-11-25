#include "Quat.h"

namespace roxlu {

Quat::Quat(float x, float y, float z, float w) 
	:w(w)
{
	v.set(x,y,z);
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
}

Mat4 Quat::getMat4() const {
	Mat4 result;
	toMat4(result);
	return result;
}

void Quat::toMat3(Mat3& dest) const {
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
	dest.m[0] = 1.0f-(tyy+tzz);
	dest.m[1] = txy-twz;
	dest.m[2] = txz+twy;
	
	dest.m[4] = txy+twz;
	dest.m[5] = 1.0f-(txx+tzz);
	dest.m[6] = tyz-twx;
	
	dest.m[8] = txz-twy;
	dest.m[9] = tyz+twx;
	dest.m[10] = 1.0f-(txx+tyy);
}

Mat3 Quat::getMat3() const {
	Mat3 result;
	toMat3(result);
	return result;
}

// angle in radians!
void Quat::rotate(const float radians, const float x, const float y, const float z) {
	Quat q;
	float ha = 0.5 * radians;
	float s = sin(ha);
	q.w = cos(ha);
	q.v.x = s * x;
	q.v.y = s * y;
	q.v.z = s * z;
	Quat cp = *this;
	cp = cp * q;
	*this = cp;
}

void Quat::setRotation(const float radians, const float x, const float y, const float z) {
	float ha = 0.5 * radians;
	float s = sin(ha);
	w = cos(ha);
	v.x = s * x;
	v.y = s * y;
	v.z = s * z;
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

// NOT VALIDATED (FROM OPENFRAMEWORKS)
Vec3 Quat::getEuler() {
	
	float test = v.x * v.y + v.z * w;
	float heading;
	float attitude;
	float bank;
	if (test > 0.499) {
		heading = 2 * atan2(v.x, w);
		attitude = PI/2;
		bank = 0;
	} else if (test < -0.499) { 
		heading = -2 * atan2(v.x, w);
		attitude = - PI/2;
		bank = 0;
	} else {
		float sqx = v.x * v.x;
		float sqy = v.y * v.y;
		float sqz = v.z * v.z;
		heading = atan2(2.0f * v.y * w - 2.0f * v.x * v.z, 1.0f - 2.0f*sqy - 2.0f*sqz);
		attitude = asin(2*test);
		bank = atan2(2.0f * v.x * w - 2.0f * v.y * v.z, 1.0f - 2.0f*sqx - 2.0f*sqz);
	}
	return Vec3(attitude, heading, bank);
	//return ofVec3f(ofRadToDeg(attitude), ofRadToDeg(heading), ofRadToDeg(bank));
}

} // roxlu
