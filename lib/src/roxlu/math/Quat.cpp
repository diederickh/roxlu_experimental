//#include "Quat.h"

#include <roxlu/math/Quat.h>

namespace roxlu {

Quat::Quat(float x, float y, float z, float w) 
	:w(w)
	,x(x)
	,y(y)
	,z(z)
{

}

void Quat::toMat4(Mat4& dest)  {
	float tx  = 2.0f*x;
	float ty  = 2.0f*y;
	float tz  = 2.0f*z;
	float twx = tx*w;
	float twy = ty*w;
	float twz = tz*w;
	float txx = tx*x;
	float txy = ty*x;
	float txz = tz*x;
	float tyy = ty*y;
	float tyz = tz*y;
	float tzz = tz*z;
	dest.m[0] = 1.0f-(tyy+tzz);
	dest.m[1] = txy-twz;
	dest.m[2] = txz+twy;
	
	dest.m[4] = txy+twz;
	dest.m[5] = 1.0f-(txx+tzz);
	dest.m[6] = tyz-twx;
	
	dest.m[8] = txz-twy;
	dest.m[9] = tyz+twx;
	dest.m[10] = 1.0f-(txx+tyy);
	return;
	
	//normalize();

	/*
	dest.m[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest.m[1] = 2.0f * x * y - 2.0f * w * z;
	dest.m[2] = 2.0f * x * z + 2.0f * w * y;
	
	dest.m[4] = 2.0f * x * y + 2.0f * w * z;
	dest.m[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest.m[6] = 2.0f * y *  z - 2.0f * w * x;
	
	dest.m[8] = 2 * x * z - 2 * w * y;
	dest.m[9] = 2 * y * z + 2 * w * x;
	dest.m[10] = 1 - 2 * x * x - 2 * y * y;
	dest.transpose();
	*/
	
	dest.m[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest.m[4] = 2.0f * x * y - 2.0f * w * z;
	dest.m[8] = 2.0f * x * z + 2.0f * w * y;
	
	dest.m[1] = 2.0f * x * y + 2.0f * w * z;
	dest.m[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest.m[9] = 2.0f * y *  z - 2.0f * w * x;
	
	dest.m[2] = 2 * x * z - 2 * w * y;
	dest.m[6] = 2 * y * z + 2 * w * x;
	dest.m[10] = 1 - 2 * x * x - 2 * y * y;
	dest.m[3]  = dest.m[7] = dest.m[11] = dest.m[12] = dest.m[13] = dest.m[14] = 0;
	dest.m[15] = 1;
	
	/*	
	// from:  http://www.j3d.org/matrix_faq/matrfaq_latest.html
	normalize();
	float xx,xy,xz,xw, yy, yz, yw, zz, zw;
	xx = x * x;
	xy = x * y;
	xz = x * z;
	yy = y * y;
	yz = y * z;
	yw = y * w;
	zz = z * z;
	zw = z * w;
	
	dest.m[0] = 1.0f - 	2.0f * (yy + zz);
	dest.m[1] =        	2.0f * (xy - zw);
	dest.m[2] =			2.0f * (xz + yw);
	
	
	dest.m[4]  =    	2 * ( xy + zw );

    dest.m[5]  = 1 - 	2 * ( xx + zz );
    dest.m[6]  =     	2 * ( yz - xw );
    
	dest.m[8]  =     	2 * ( xz - yw );
    dest.m[9]  =     	2 * ( yz + xw );
	printf("x,y,z: %f, %f, %f\n", x,y,z);
	printf("9: yz:%f, xw:%f", yz, xw);
    dest.m[10] = 1 - 	2 * ( xx + yy );
	
    dest.m[3]  = dest.m[7] = dest.m[11] = dest.m[12] = dest.m[13] = dest.m[14] = 0;
    dest.m[15] = 1;
	*/
	
	// from essiantial mathematics for game developers
	
	/*
	normalize();
	float xs, ys, zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;
	xs = x + x;
	ys = y + y;
	zs = z + z;
	
	wx = w * xs;
	wy = w * ys;
	wz = w * zs;
	
	xx = x * xs;
	xy = x * ys;
	xz = x * zs;
	
	yy = y * ys;
	yz = y * zs;
	zz = z * zs;
	
	dest.m[0] = 1.0f - (yy + zz);
	dest.m[1] = xy - wz;
	dest.m[2] = xz + wy;
	
	dest.m[4] = xy + wz;
	dest.m[5] = 1.0f - (xx + zz);
	dest.m[6] = yz - wx;
	
	dest.m[8] = xz - wy;
	dest.m[9] = yz + wx;
	dest.m[10] = 1.0 - (xx + yy);
	*/
}

Mat4 Quat::getMat4()  {
	Mat4 result;
	toMat4(result);
	return result;
}

void Quat::toMat3(Mat3& dest) {
	// from essiantial mathematics for game developers
	normalize();
	float xs, ys, zs, wx,wy,wz, xx,xy,xz, yy,yz,zz;
	xs = x + x;
	ys = y + y;
	zs = z + z;
	
	wx = w * xs;
	wy = w * ys;
	wz = w * zs;
	
	xx = x * xs;
	xy = x * ys;
	xz = x * zs;
	
	yy = y * ys;
	yz = y * zs;
	
	zz = z * zs;
	
	dest.m[0] = 1.0f - (yy + zz);
	dest.m[1] = xy - wz;
	dest.m[2] = xz + wy;
	
	dest.m[3] = xy + wz;
	dest.m[4] = 1.0f - (xx + zz);
	dest.m[5] = yz - wx;
	
	dest.m[6] = xz - wy;
	dest.m[7] = yz + wx;
	dest.m[8] = 1.0 - (xx + yy);
}

Mat3 Quat::getMat3() {
	Mat3 result;
	toMat3(result);
	return result;
}

const Mat3 Quat::getMat3() const {
	return getMat3();
}


// angle in radians!
void Quat::rotate(const float radians, const float xx, const float yy, const float zz) {
	Quat q;
	float ha = 0.5 * radians;
	float s = sin(ha);
	q.w = cos(ha);
	q.x = s * xx;
	q.y = s * yy;
	q.z = s * zz;
	Quat cp = *this;
	cp = cp * q;
	*this = cp;
}


void Quat::setRotation(const float a, const Vec3 axis) {
	setRotation(a, axis.x, axis.y, axis.z);
}
	
void Quat::setRotation(const float radians, const float xx, const float yy, const float zz) {
	float ha = 0.5 * radians;
	float s = sin(ha);
	w = cos(ha);
	x = s * xx;
	y = s * yy;
	z = s * zz;
}


Vec3 Quat::transform(const Vec3& rVec) const {
	float vmult = (2.0f * x * rVec.x + y * rVec.y + z * rVec.z);
	float cross_mult = 2.0f * w;
	float pmult = cross_mult*w - 1.0f;
	return Vec3(
		pmult * rVec.x + vmult * x + cross_mult * (y * rVec.z - z * rVec.y)
		,pmult * rVec.y + vmult * y + cross_mult * (z * rVec.x - x * rVec.z)
		,pmult * rVec.z + vmult * z + cross_mult * (x * rVec.y - y * rVec.x)
	);
}

void Quat::normalize() {
	float len = w*w + x*x + y*y + z*z;
	float factor = 1.0f / sqrt(len);
	x *= factor;
	y *= factor;
	z *= factor;
	w *= factor;
}

// NOT VALIDATED (FROM OPENFRAMEWORKS)
Vec3 Quat::getEuler() {
	
	float test = x * y + z * w;
	float heading;
	float attitude;
	float bank;
	if (test > 0.499) {
		heading = 2 * atan2(x, w);
		attitude = PI/2;
		bank = 0;
	} else if (test < -0.499) { 
		heading = -2 * atan2(x, w);
		attitude = - PI/2;
		bank = 0;
	} else {
		float sqx = x * x;
		float sqy = y * y;
		float sqz = z * z;
		heading = atan2(2.0f * y * w - 2.0f * x * z, 1.0f - 2.0f*sqy - 2.0f*sqz);
		attitude = asin(2*test);
		bank = atan2(2.0f * x * w - 2.0f * y * z, 1.0f - 2.0f*sqx - 2.0f*sqz);
	}
	return Vec3(attitude, heading, bank);
	//return ofVec3f(ofRadToDeg(attitude), ofRadToDeg(heading), ofRadToDeg(bank));
}

} // roxlu
