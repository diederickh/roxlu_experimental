#ifndef VEC4H
#define VEC4H

#include <iostream>
#include <iomanip>

using std::setw;

class Vec4 {
public:
	inline Vec4()
		:x(0.0f)
		,y(0.0f)
		,z(0.0f)
		,w(0.0f)
	{
	}
	
	inline Vec4(float xx, float yy, float zz, float ww)
		:x(xx)
		,y(yy)
		,z(zz)
		,w(ww)
	{
	}
	
	inline void set(float xx, float yy, float zz, float ww);
	inline float* getPtr() { return &x; }
	
	friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
		
	float x;
	float y;
	float z;
	float w;
};

inline void Vec4::set(float xx, float yy, float zz, float ww) {
	x = xx;
	y = yy;
	z = zz;
	w = ww;
}

inline std::ostream& operator<<(std::ostream& os, const Vec4& v) {
	int w = 6;
	os 	<< setw(w) << v.x <<", " 
		<< setw(w) << v.y << ", " 
		<< setw(w) << v.z << ", " 
		<< setw(w) << v.w;
	return os;
}

#endif