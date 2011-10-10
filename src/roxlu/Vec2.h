#pragma once

// If you're smarter then the optimizer!
//------------------------------------------------------------------------------
#define dot2(a,b,r) 		r = (a.x * b.x) + (a.y * b.y);

#define isqrt2(a,odist)  	odist=(float)(a.x*a.x+a.y*a.y); \
		     	          	float rsf=odist*0.5f; \
							long rsl=*(long*)&odist; \
							const float rsopf = 1.5f; \
							rsl=0x5f3759df-(rsl>>1); \
							odist=*(float*)&rsl; \
	 			            odist=odist*(rsopf-(rsf*odist*odist));  \
						
//#define length2(a,r)		isqrt2(a,r); \
//							r = 1/r;
#define copy2(a,b)			b.x = a.x; b.y = a.y;;					
#define set2(a,x,y)			a.x = x; a.y = y;;
#define normalize2(a,l,b)	l = 0.0; \
							isqrt2(a,l); \
							b.x = a.x * l; \
							b.y = a.y * l;
							

	
#define	subtract2(a,b,c)	c.x = a.x - b.x; c.y = a.y - b.y;
#define	add2(a,b,c)			c.x = a.x + b.x; c.y = a.y + b.y;;
#define multiply2(a,b,c)	c.x = a.x * b; c.y = a.y * b;
#define zero2(a)			a.x = 0; a.y = 0; 
#define divide2(a,f,r)		r.x = a.x / f; r.y = a.y / f; 

#include <iostream>
#include <math.h>

struct Vec2 {
public:
	Vec2(float nX = 0.0f, float nY = 0.0f)
		:x(nX)
		,y(nY)
	{
	}
			
	// Basic vector math.
	inline void 	set(float nX, float nY);
	inline float 	length();
	inline float 	lengthSquared();
	inline float*	getPtr() { return &x; }
	
	// Accessors
	float& operator[](unsigned int nIndex) {
		return (&x)[nIndex];
	}
	
	float operator[](unsigned int nIndex) const {
		return (&x)[nIndex];
	}
	
	// Comparison
	bool operator==(const Vec2& rOther) const;
	bool operator!=(const Vec2& rOther) const;
	
	// Operators
	Vec2 	operator-(const Vec2& rOther) const;
	Vec2 	operator+(const Vec2& rOther) const;
	Vec2& 	operator-=(const Vec2& rOther);
	Vec2& 	operator+=(const Vec2& rOther);
	Vec2 	operator*(const float nScalar) const;
	Vec2&	operator*=(const float nScalar);
	Vec2 	operator/(const float nScalar) const;
	Vec2& 	operator/=(const float nScalar);
			
	// Streams
	friend std::ostream& operator<<(std::ostream& os, const Vec2& rVec);		
	friend std::istream& operator>>(std::istream& is, Vec2& rVec);		
			
	float x;
	float y;
	float z;
};

// -----------------------------------------------------------------------------
inline void Vec2::set(float nX, float nY) {
	x = nX; 
	y = nY;
}

inline float Vec2::length() {
	return sqrt(x*x + y*y);
}

inline float Vec2::lengthSquared() {
	return x*x + y*y;
}

// -----------------------------------------------------------------------------
inline bool Vec2::operator==(const Vec2& rOther) const {
	return x == rOther.x && y == rOther.y;
}

inline bool Vec2::operator!=(const Vec2& rOther) const {
	return x != rOther.x || y != rOther.y;
}

inline Vec2 Vec2::operator-(const Vec2& rOther) const {
	return Vec2(x-rOther.x, y-rOther.y);
}

inline Vec2 Vec2::operator+(const Vec2& rOther) const {
	return Vec2(x+rOther.x, y+rOther.y);
}

inline Vec2& Vec2::operator-=(const Vec2& rOther) {
	x -= rOther.x;
	y -= rOther.y;
	return *this;
}

inline Vec2& Vec2::operator+=(const Vec2& rOther) {
	x += rOther.x;
	y += rOther.y;
	return *this;
}

inline Vec2 Vec2::operator*(const float nScalar) const {
	return Vec2(x*nScalar, y*nScalar);
}

inline Vec2& Vec2::operator*=(const float nScalar) {
	x *= nScalar;
	y *= nScalar;
	return *this;
}

inline Vec2 Vec2::operator/(const float nScalar) const {
	float inv = 1.0f/nScalar;
	return Vec2(x*inv, y*inv);
}

inline Vec2& Vec2::operator/=(const float nScalar) {
	float inv = 1.0f/nScalar;
	x *= inv;
	y *= inv;
	return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Vec2& rVec) {
	os << rVec.x << ", " << rVec.y;
	return os;
}

inline std::istream& operator>>(std::istream& is, Vec2& rVec) {
	is >> rVec.x;
	is.ignore(2);
	is >> rVec.y;
	return is;
}