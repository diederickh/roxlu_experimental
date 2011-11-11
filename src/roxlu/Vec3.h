#ifndef ROXLU_VEC3H
#define ROXLU_VEC3H

// When you're smarter than the compiler optimizer
// -----------------------------------------------------------------------------
#define dot3(a,b,r) 		r = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
#define cross3(a,b,dest)	dest.x = (a.y * b.z) - (b.y * a.z); \
							dest.y = (a.z * b.x) - (b.z * a.x); \
							dest.z = (a.x * b.y) - (b.x * a.y);


#define isqrt3(a,odist)  { \
							odist=(float)(a.x*a.x+a.y*a.y + a.z * a.z); \
		     	          	float rsf=odist*0.5f; \
							long rsl=*(long*)&odist; \
							const float rsopf = 1.5f; \
							rsl=0x5f3759df-(rsl>>1); \
							odist=*(float*)&rsl; \
	 			            odist=odist*(rsopf-(rsf*odist*odist));  \
							}
#define length3(a,r)		isqrt3(a,r); \
							r = 1/r;
#define copy3(a,b)			b.x = a.x; b.y = a.y; b.z = a.z;					
#define set3(a,x,y,z)		a.x = x; a.y = y; a.z = z;
#define normalize3(a,l,b)	l = 0.0; \
							isqrt3(a,l); \
							b.x = a.x * l; \
							b.y = a.y * l; \
							b.z = a.z * l; 
							

#define rotate3(a, angle, ax, result)	\
							{ \
							float sina = sin(angle); \
							float cosa = cos(angle); \
							float cosb = 1.0f - cosa; \
							result.x = a.x * (ax.x  * ax.x * cosb + cosa) + a.y * (ax.x * ax.y * cosb - ax.z * sina) + a.z * (ax.x * ax.z * cosb + ax.y * sina);   \
							result.y = a.x * (ax.y  * ax.x * cosb + ax.z * sina) + a.y * (ax.y * ax.y * cosb + cosa) + a.z * (ax.y * ax.z * cosb - ax.x * sina);   \
							result.z = a.x * (ax.z  * ax.x * cosb - ax.y * sina) + a.y * (ax.z * ax.y * cosb + ax.x * sina) + a.z * (ax.z * ax.z * cosb + cosa);   \
							}
	
#define	subtract3(a,b,c)	c.x = a.x - b.x; c.y = a.y - b.y; c.z = a.z - b.z;
#define	add3(a,b,c)			c.x = a.x + b.x; c.y = a.y + b.y; c.z = a.z + b.z;
#define multiply3(a,b,c)	c.x = a.x * b; c.y = a.y * b; c.z = a.z * b;
#define zero3(a)			a.x = 0; a.y = 0; a.z = 0;
#define divide3(a,f,r)		r.x = a.x / f; r.y = a.y / f; r.z = a.z / f;
#define scale3(vec,s,l,r)	normalize3(vec,l,vec); r.x = vec.x * s; r.y  = vec.y * s; r.z = vec.z * s;
#define ptr3(vec)			&(vec).x

#include <iostream>
#include <math.h>


namespace roxlu {

class Vec3;
class Mat3;

Vec3 cross(const Vec3& a, const Vec3& b);
float dot(const Vec3& a, const Vec3& b);

// Vector 3 class.
// -----------------------------------------------------------------------------
struct Vec3 {
	friend class Mat3;
	Vec3(float nX = 0.0f, float nY = 0.0f, float nZ = 0.0f)
		:x(nX)
		,y(nY)
		,z(nZ)
	{
	}
	
	Vec3(const Vec3& other) 
		:x(other.x)
		,y(other.y)
		,z(other.z)
	{
	}

	// Basic methods.	
	inline Vec3&	rotate(float angle, float x, float y, float z);
	inline Vec3& 	rotate(float angle, Vec3 axis);
	inline Vec3& 	rotate(float angle, Vec3 axis, Vec3 pivot);
	inline void 	set(float nX, float nY, float nZ);
	inline float 	length();
	inline float 	lengthSquared();
	inline float 	lengthSquared(const Vec3& other);
	inline Vec3 	getScaled(float length);
	inline Vec3&	scale(float length);
	inline Vec3&	normalize();
	inline Vec3 	getNormalized();
	inline float 	dot(const Vec3& vec);
	inline Vec3& 	cross(const Vec3& vec);
	inline Vec3 	getCrossed(const Vec3& vec);
	inline float* 	getPtr() { return &x; };
	inline void		print();

	// Accessors
	float& operator[](unsigned int nIndex) {
		return (&x)[nIndex];
	}
	
	float operator[](unsigned int nIndex) const {
		return (&x)[nIndex];
	}
	

	// Comparison
	bool operator==(const Vec3& rOther) const;
	bool operator!=(const Vec3& rOther) const;
	
	// Operators
	inline Vec3 	operator-(const Vec3& rOther) const;
	inline Vec3 	operator+(const Vec3& rOther) const;
	inline Vec3& 	operator-=(const Vec3& rOther);
	inline Vec3& 	operator+=(const Vec3& rOther);
	inline Vec3 	operator*(const float nScalar) const;
	inline Vec3& 	operator*=(const float nScalar);
	inline Vec3 	operator/(const float nScalar) const;
	inline Vec3&	operator/=(const float nScalar);
			
	// Streams
	friend std::ostream& operator<<(std::ostream& os, const Vec3& rVec);		
	friend std::istream& operator>>(std::istream& is, Vec3& rVec);		
			
	float x;
	float y;
	float z;
};

// -----------------------------------------------------------------------------

inline void Vec3::set(float nX, float nY, float nZ) {
	x = nX; 
	y = nY;
	z = nZ;
}

inline float Vec3::length() {
	return sqrt(x*x + y*y + z*z);
}

inline Vec3& Vec3::normalize() {
	float l = length();
	x/= l;
	y/= l;
	z/=l;
	return *this;
	/*
	float il = 0;
	if(l > 0) {
		il = 1/l;
	}
	
	x *= il;
	y *= il;
	z *= il;
	
//	x /= il;
//	y /= il;
//	z /= il;
	return *this;
	*/
}

inline Vec3 Vec3::getNormalized() {
	float l = length();
	float il = 0;
	if(l > 0) {
		il = 1/l;
	}
	return Vec3(x*il, y*il, z*il);
}

inline float Vec3::lengthSquared() {
	return x*x + y*y + z*z;
}

inline float Vec3::lengthSquared(const Vec3& other) {
	float dx = other.x - x;
	float dy = other.y - y;
	float dz = other.z - z;
	return dx*dx + dy*dy + dz*dz;
}

inline Vec3& Vec3::rotate(float angle, float x, float y, float z) {
	return rotate(angle, Vec3(x,y,z));
}

inline Vec3& Vec3::rotate(float a, Vec3 ax) {
	float sina = sin(a);
	float cosa = cos(a);
	float cosb = 1.0f - cosa;
	Vec3 v( x*(ax.x*ax.x*cosb + cosa)
				   + y*(ax.x*ax.y*cosb - ax.z*sina)
				   + z*(ax.x*ax.z*cosb + ax.y*sina),
				   x*(ax.y*ax.x*cosb + ax.z*sina)
				   + y*(ax.y*ax.y*cosb + cosa)
				   + z*(ax.y*ax.z*cosb - ax.x*sina),
				   x*(ax.z*ax.x*cosb - ax.y*sina)
				   + y*(ax.z*ax.y*cosb + ax.x*sina)
				   + z*(ax.z*ax.z*cosb + cosa) );
	
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


inline Vec3& Vec3::rotate(float a, Vec3 axis, Vec3 pivot) {
	Vec3 ax = axis.getNormalized();
	x -= pivot.x;
	y -= pivot.y;
	z -= pivot.z;

	float sina = sin( a );
	float cosa = cos( a );
	float cosb = 1.0f - cosa;

	float xrot = x*(ax.x*ax.x*cosb + cosa)
				 + y*(ax.x*ax.y*cosb - ax.z*sina)
				 + z*(ax.x*ax.z*cosb + ax.y*sina);
	
	float yrot = x*(ax.y*ax.x*cosb + ax.z*sina)
				 + y*(ax.y*ax.y*cosb + cosa)
				 + z*(ax.y*ax.z*cosb - ax.x*sina);
				 
	float zrot = x*(ax.z*ax.x*cosb - ax.y*sina)
				 + y*(ax.z*ax.y*cosb + ax.x*sina)
				 + z*(ax.z*ax.z*cosb + cosa);

	x = xrot + pivot.x;
	y = yrot + pivot.y;
	z = zrot + pivot.z;

	return *this;
}

// 0-1, 0.5 is center
inline Vec3 Vec3::getScaled(float scale) {
	return Vec3(x*scale, y*scale, z*scale);
	/*
	float l = (float)sqrt(x*x+y*y+z*z);
	if(l > 0) {
		float il = 1/l;
		return Vec3(x *il*length, y*il*length, z*il*length);
	}
	else {
		return Vec3();
	}
	*/
}
// 0-1, 0.5 is center
inline Vec3& Vec3::scale(float scale) {
	//float l = (float)sqrt(x*x+y*y+z*z);
	//if(l > 0) {
	//	float il = 1/l;
		
	//}
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

inline float Vec3::dot(const Vec3& vec) {
	return x*vec.x + y*vec.y + z*vec.z;
}

inline Vec3& Vec3::cross(const Vec3& vec) {
	float tmp_x = y * vec.z - z*vec.y;
	float tmp_y = z * vec.x - x*vec.z;
	z = x * vec.y - y * vec.x;
	x = tmp_x;
	y = tmp_y;
	return *this;
}

inline Vec3 Vec3::getCrossed(const Vec3& vec) {
	return Vec3(
		 y * vec.z - z * vec.y
		,z * vec.x - x * vec.z
		,x * vec.y - y * vec.x
	);
}

inline void Vec3::print() {
	printf("%f, %f, %f\n", x, y, z);
}

// -----------------------------------------------------------------------------
inline bool Vec3::operator==(const Vec3& rOther) const {
	return x == rOther.x && y == rOther.y && z == rOther.z;
}

inline bool Vec3::operator!=(const Vec3& rOther) const {
	return x != rOther.x || y != rOther.y || z != rOther.z;
}

inline Vec3 Vec3::operator-(const Vec3& rOther) const {
	return Vec3(x-rOther.x, y-rOther.y, z-rOther.z);
}

inline Vec3 Vec3::operator+(const Vec3& rOther) const {
	return Vec3(x+rOther.x, y+rOther.y, z+rOther.z);
}

inline Vec3& Vec3::operator-=(const Vec3& rOther) {
	x -= rOther.x;
	y -= rOther.y;
	z -= rOther.z;
	return *this;
}


inline Vec3& Vec3::operator+=(const Vec3& rOther) {
	x += rOther.x;
	y += rOther.y;
	z += rOther.z;
	return *this;
}

inline Vec3 Vec3::operator*(const float nScalar) const {
	return Vec3(x*nScalar, y*nScalar, z*nScalar);
}

inline Vec3& Vec3::operator*=(const float nScalar) {
	x *= nScalar;
	y *= nScalar;
	z *= nScalar;
	return *this;
}

inline Vec3 Vec3::operator/(const float nScalar) const {
	float inv = 1.0f/nScalar;
	return Vec3(x*inv, y*inv, z*inv);
}

inline Vec3& Vec3::operator/=(const float nScalar) {
	float inv = 1.0f/nScalar;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& rVec) {
	os << rVec.x << ", " << rVec.y << ", " << rVec.z;
	return os;
}

inline std::istream& operator>>(std::istream& is, Vec3& rVec) {
	is >> rVec.x;
	is.ignore(2);
	is >> rVec.y;
	is.ignore(2);
	is >> rVec.z;
	return is;
}

// non members
// -----------------------------------------------------------------------------
inline Vec3 operator+( float f, const Vec3& vec ) {
    return Vec3( f+vec.x, f+vec.y, f+vec.z );
}

inline Vec3 operator-( float f, const Vec3& vec ) {
    return Vec3( f-vec.x, f-vec.y, f-vec.z );
}

inline Vec3 operator/( float f, const Vec3& vec ) {
    return Vec3( f/vec.x, f/vec.y, f/vec.z);
}
inline Vec3 operator*(float f, const Vec3& vec) {
    return Vec3(f*vec.x, f*vec.y, f*vec.z );
}


// -----------------------------------------------------------------------------
inline Vec3 cross(const Vec3& a, const Vec3& b) {
	Vec3 p;
	cross3(a,b,p);
	return p;
}

inline float dot(const Vec3& a, const Vec3& b) {
	float r = 0.0;
	dot3(a,b,r);
	return r;
}

} // roxlu

#endif