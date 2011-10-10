#ifndef ROXLU_QAUTH
#define ROXLU_QAUTH

#include "Mat4.h"
#include "Vec3.h"

#include <iostream>
#include <iomanip>


class Quat {
public:	
	Vec3 v;
	float w;
	
	Quat();
	void normalize();
	void toMat4(Mat4& aDest) const;
	Mat4 getMat4() const;
	void fromAngleAxis(float nAngle, float nX, float nY, float nZ);
	void rotate(float nAngle, const float nX, const float nY, const float nZ);
	void rotate(float nAngle, const Vec3& rAxis) { rotate(nAngle, rAxis.x, rAxis.y, rAxis.z); }
	Vec3 rotate(const Vec3& rVec) const;
	
	Quat operator*(const Quat& rOther) const;
	Quat operator*(float nScalar) const;
	Quat& operator*=(const Quat& rOther);
	Quat operator+(const Quat& rOther) const;
	Quat operator-(const Quat& rOther) const;
	
	friend std::ostream& operator<<(std::ostream& os, const Quat& rQuat);
	
};

inline std::ostream& operator<<(std::ostream& os, const Quat& rQuat) {
	os << rQuat.v.x << ", " << rQuat.v.y << ", " << rQuat.v.z <<", " << rQuat.w;
	return os;
}

inline Quat Quat::operator*(const Quat& rOther) const {
	Quat q_out;
	q_out.w = w * rOther.w - v.x * rOther.v.x - v.y * rOther.v.y - v.z * rOther.v.z;
	q_out.v.x = w * rOther.v.x + v.x * rOther.w + v.y * rOther.v.z - v.z * rOther.v.y;
	q_out.v.y = w * rOther.v.y + v.y * rOther.w + v.z * rOther.v.x - v.x * rOther.v.z;
	q_out.v.z = w * rOther.v.z + v.z * rOther.w + v.x * rOther.v.y - v.y * rOther.v.x;
	return q_out;
}


inline Quat& Quat::operator*=(const Quat& rOther) {
	w = w * rOther.w - v.x * rOther.v.x - v.y * rOther.v.y - v.z * rOther.v.z;
	v.x = w * rOther.v.x + v.x * rOther.w + v.y * rOther.v.z - v.z * rOther.v.y;
	v.y = w * rOther.v.y + v.y * rOther.w + v.z * rOther.v.x - v.x * rOther.v.z;
	v.z = w * rOther.v.z + v.z * rOther.w + v.x * rOther.v.y - v.y * rOther.v.x;
	return *this;
}


inline Quat Quat::operator*(float nScalar) const {
	Quat q_out;
	q_out.v = v * nScalar;
	q_out.w = w * nScalar;
	return q_out;
}

inline Quat Quat::operator+(const Quat& rOther) const {
	Quat q_out;
	q_out.v = v + rOther.v;
	q_out.w = w + rOther.w;
	return q_out;
}

inline Quat Quat::operator-(const Quat& rOther) const {
	Quat q_out;
	q_out.v = v - rOther.v;
	q_out.w = w - rOther.w;
	return q_out;
}

#endif