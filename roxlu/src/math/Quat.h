#ifndef ROXLU_QAUTH
#define ROXLU_QAUTH

#include "Mat3.h"
#include "Mat4.h"
#include "Vec3.h"
#include "../core/Constants.h"

namespace roxlu {

class Quat {
public:	
	float x,y,z,w;
	Quat(float x = 0, float y = 0, float z = 0, float w = 1);
	void set(const float& x, const float& y, const float& z, const float& w);
	void normalize();
	void toMat4(Mat4& aDest);
	Mat4 getMat4() ;
	void toMat3(Mat3& dest);
	Mat3 getMat3();
	const Mat3 getMat3() const;
	void fromMat3(const Mat3& src);
	
	// absolute rotations. (euler)
	void setRotation(const float radians, const float x, const float y, const float z);
	void setRotation(const float a, const Vec3 axis);
	inline void setXRotation(const float radians);
	inline void setYRotation(const float radians);
	inline void setZRotation(const float radians);
	
	// relative rotation
	void rotate(const float radians, const float x, const float y, const float z); 
	void rotate(const float radians, const Vec3& rAxis) { rotate(radians, rAxis.x, rAxis.y, rAxis.z); }
	Vec3 transform(const Vec3& rVec) const;
	
	Quat operator*(const Quat& rOther) const;
	Quat operator*(float nScalar) const;
	Quat& operator*=(const Quat& rOther);
	Quat operator+(const Quat& rOther) const;
	Quat operator-(const Quat& rOther) const;
	float& operator[](unsigned int i) { return (&x)[i]; }
	void inverse();
	Vec3 getEuler();
	
	inline void print();
	
	friend std::ostream& operator<<(std::ostream& os, const Quat& rQuat);
	
};

inline void Quat::set(const float& x, const float& y, const float& z, const float& w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline void Quat::setXRotation(const float radians) {
	setRotation(radians, 1, 0, 0);
}

inline void Quat::setYRotation(const float radians) {
	setRotation(radians, 0, 1, 0);
}

inline void Quat::setZRotation(const float radians) {
	setRotation(radians, 0, 0, 1);
}

inline std::ostream& operator<<(std::ostream& os, const Quat& rQuat) {
	os << rQuat.x << ", " << rQuat.y << ", " << rQuat.z <<", " << rQuat.w;
	return os;
}

inline Quat Quat::operator*(const Quat& rOther) const {
	Quat q_out;
	q_out.w = w * rOther.w - x * rOther.x - y * rOther.y - z * rOther.z;
	q_out.x = w * rOther.x + x * rOther.w + y * rOther.z - z * rOther.y;
	q_out.y = w * rOther.y + y * rOther.w + z * rOther.x - x * rOther.z;
	q_out.z = w * rOther.z + z * rOther.w + x * rOther.y - y * rOther.x;
	return q_out;
}


inline Quat& Quat::operator*=(const Quat& rOther) {
	w = w * rOther.w - x * rOther.x - y * rOther.y - z * rOther.z;
	x = w * rOther.x + x * rOther.w + y * rOther.z - z * rOther.y;
	y = w * rOther.y + y * rOther.w + z * rOther.x - x * rOther.z;
	z = w * rOther.z + z * rOther.w + x * rOther.y - y * rOther.x;
	return *this;
}


inline Quat Quat::operator*(float nScalar) const {
	Quat q_out;
	q_out.x = x * nScalar;
	q_out.y = y * nScalar;
	q_out.z = z * nScalar;
	q_out.w = w * nScalar;
	return q_out;
}

inline Quat Quat::operator+(const Quat& rOther) const {
	Quat q_out;
	q_out.x = x + rOther.x;
	q_out.y = y + rOther.y;
	q_out.z = z + rOther.z;
	q_out.w = w + rOther.w;
	return q_out;
}

inline Quat Quat::operator-(const Quat& rOther) const {
	Quat q_out;
	q_out.x = x - rOther.x;
	q_out.y = y - rOther.y;
	q_out.z = z - rOther.z;
	q_out.w = w - rOther.w;
	return q_out;
}


// from GLM
inline void Quat::fromMat3(const Mat3& src) {
	bool debug = false;
	float four_x_sq_min1 = src.m[0] - src.m[4] - src.m[8];
	float four_y_sq_min1 = src.m[4] - src.m[0] - src.m[8];
	float four_z_sq_min1 = src.m[8] - src.m[0] - src.m[4];
	float four_w_sq_min1 = src.m[0] + src.m[4] + src.m[8];
	
	int biggest_index = 0;
	float four_biggest_sq_min1 = four_w_sq_min1;
	//printf("four biggest: %f\n", four_biggest_sq_min1);
	if(four_x_sq_min1 > four_biggest_sq_min1) {
		four_biggest_sq_min1 = four_x_sq_min1;
		biggest_index = 1;
	}
	
	if(four_y_sq_min1 > four_biggest_sq_min1) {
		four_biggest_sq_min1 = four_y_sq_min1;
		biggest_index = 2;
	}
	
	if(four_z_sq_min1 > four_biggest_sq_min1) {
		four_biggest_sq_min1 = four_z_sq_min1;
		biggest_index = 3;
	}
	
	float biggest_val = sqrt(four_biggest_sq_min1 + 1.0) * 0.5;
	float mult = 0.25 / biggest_val;
	switch(biggest_index) {
		case 0: {
			w = biggest_val;
			x = (src.m[7] - src.m[5]) * mult;
			y = (src.m[2] - src.m[6]) * mult;
			z = (src.m[3] - src.m[1]) * mult;
			break;
		}
		case 1: {
			w = (src.m[7] - src.m[2]) * mult;
			x = biggest_val;
			y = (src.m[3] + src.m[1]) * mult;
			z = (src.m[2] + src.m[6]) * mult;
			break;
		}
		case 2: {
			if(debug) {
				printf("2: %f, 6:%f <---+++\n", src.m[2], src.m[6]);
			}
			w = (src.m[2] - src.m[6]) * mult;
			x = (src.m[3] + src.m[1]) * mult;
			y = biggest_val;
			z = (src.m[7] + src.m[5]) * mult;
			break;
		}
		case 3: {
			w = (src.m[3] - src.m[1]) * mult;
			x = (src.m[2] + src.m[6]) * mult;
			y = (src.m[7] + src.m[2]) * mult;
			z = biggest_val;
			break;
		}
	};
	
	
	// float biggestVal = sqrt(fourBiggestSquaredMinus1 + typename detail::tquat<T>::value_type(1)) * typename detail::tquat<T>::value_type(0.5);
     // float mult = typename detail::tquat<T>::value_type(0.25) / biggestVal;
	 if(debug) {
		printf("BIGGEST INDEX: %d\n", biggest_index);
		printf("BIGGEST VAL: %f\n", biggest_val);
		printf("MULT: %f\n", mult);
		printf("X = %f, Y = %f, Z = %f, W = %f\n", x,y,z, w);
	}
//	printf("%f, %f, %f, %f <-------\n", four_x_sq_min1, four_y_sq_min1,four_z_sq_min1,four_w_sq_min1);
	//src.print();
/*

// from math for computer game developers
	float trace = src.trace();
	if(trace > 0.0f) {
		float s = sqrtf(trace+1.0f);
		w = s * 0.5f;
		float recip = 0.5f/s;
		x = (src(2,1) - src(1,2)) * recip;
		y = (src(0,2) - src(2,0)) * recip;
		z = (src(1,0) - src(0,1)) * recip;
	}
	else {
		unsigned int i = 0;
		if(src(1,1) > src(0,0)) {
			i = 1;
		}
		if(src(2,2) > src(i,i)) {
			i = 2;
		}
		unsigned int j = (i+1)%3;
		unsigned int k = (j+1)%3;
		float s = sqrtf(src(i,i) - src(j,j) - src(k,k)+1.0f);
		(*this)[i] = 0.5f * s;
		float recip = 0.5f / s;
		float w = (src(k,j) - src(j,k)) * recip;
		(*this)[j] = (src(j,i) + src(i,j)) * recip;
		(*this)[k] = (src(k,i) + src(i,k)) * recip;
	}
	*/
}


inline void Quat::print() {
	printf("%3.3f %3.3f %3.3f %3.3f\n", x, y, z, w);
}

inline void Quat::inverse() {
	x = -x;
	y = -y;
	z = -z;
}

} // roxlu
#endif