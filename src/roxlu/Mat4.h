#ifndef MAT4H
#define MAT4H

#include <iostream>
#include <iomanip>

#include "Vec3.h"
#include "Vec4.h"

using std::ostream;

class Mat3;

class Mat4 {
public:	
	inline Mat4();
	
	// copy
	Mat4(const Mat4& o);
	Mat4& operator=(const Mat4& o);
		
	bool operator==(const Mat4& o) const;
	bool operator!=(const Mat4& o) const;
	
	bool isZero();
	bool isIdentity();
	void clean();
	void identity();
	
	Mat4& affineInverse();	 
	Mat4& inverse();
	Mat4& transpose();
	Mat4& translate(const Vec3& v);
	Mat4& translate(float x, float y, float z);

	Mat4& translation(float x, float y, float z);
	Mat4& rotation(const Mat3& o);
	Mat4& rotation(float angleX, float angleY, float angeZ);
	Mat4& rotation(float a, float x, float y, float z);
	Mat4& rotationX(float a); // rotate around x-axis
	Mat4& rotationY(float a); // rotate around y-axis
	Mat4& rotationZ(float a); // rotate around z-axis
	Mat4& scale(float x, float y, float z);
	Mat4& scaling(float x, float y, float z);
	
	Mat4& frustum(float l, float r, float b, float t, float n, float f);
	Mat4& frustumCenter(float w, float h, float n, float f);
	Mat4& perspective(float fov, float aspect, float near, float far);
	Mat4& ortho(float l, float r, float b, float t, float n, float f);
	Mat4& orthoCenter(float w, float h, float n, float f);
	Mat4& orthoTopLeft(float w, float h, float n, float f);
	Mat4& orthoBottomLeft(float w, float h, float n, float f);	
	
	// rotate and translate
	inline Vec3 transform(const Vec3& v) const;

	Mat4& operator+=(const Mat4& o);
	Mat4  operator+(const Mat4& o) const;
	Mat4& operator-=(const Mat4& o);
	Mat4  operator-(const Mat4& o) const;
	Mat4  operator-() const;
	Mat4  operator*(const Mat4& o) const;	
	Mat4& operator*=(const Mat4& o);
	Mat4& operator*=(float s);
	Mat4  operator*(float s);
	Vec4  operator*(const Vec4& v) const;
	
	inline float* getPtr() { return &m[0]; }
	
	float m[16];
	
	// stream operator
	friend ostream& operator<<(ostream& os, const Mat4& o);
};

extern Mat4 operator*(float s, const Mat4& o);
extern Mat4 affine_inverse(const Mat4& o);
extern Mat4 transpose(const Mat4& o);
//extern int mat4_inverse(float *m, float *out) ;
extern int mat4_inverse(const Mat4& o, Mat4& out);

inline Mat4::Mat4() {
	m[0]  = 1.0f;
	m[1]  = 0.0f;
	m[2]  = 0.0f;
	m[3]  = 0.0f;
	
	m[4]  = 0.0f;
	m[5]  = 1.0f;
	m[6]  = 0.0f;
	m[7]  = 0.0f;
	
	m[8]  = 0.0f;
	m[9]  = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;
	
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

inline ostream& operator<<(ostream& os, const Mat4& o) {
	int w = 8;
	os 	<< 	std::setw(w)
		<<	o.m[0] 		<< ", " << std::setw(w) 
		<<	o.m[4] 		<< ", " << std::setw(w) 
		<<	o.m[8] 		<< ", " << std::setw(w) 
		<<	o.m[12] 	<< ", " << std::setw(w)
		<<	std::endl;
		
	os 	<< 	std::setw(w)
		<<	o.m[1] 		<< ", " << std::setw(w) 
		<<	o.m[5] 		<< ", " << std::setw(w) 
		<<	o.m[9] 		<< ", " << std::setw(w) 
		<<	o.m[13] 	<< ", " << std::setw(w)
		<<	std::endl;
		
		
	os 	<< 	std::setw(w)
		<<	o.m[2] 		<< ", " << std::setw(w) 
		<<	o.m[6] 		<< ", " << std::setw(w) 
		<<	o.m[10] 	<< ", " << std::setw(w) 
		<<	o.m[14] 	<< ", " << std::setw(w)
		<<	std::endl;
		
	os 	<< 	std::setw(w)
		<<	o.m[3] 		<< ", " << std::setw(w) 
		<<	o.m[7] 		<< ", " << std::setw(w) 
		<<	o.m[11] 	<< ", " << std::setw(w) 
		<<	o.m[15] 	<< ", " << std::setw(w)
		<<	std::endl;
	return os;
}


inline Vec3 Mat4::transform(const Vec3& o) const {
	Vec3 r;
	r.x = m[0] * o.x + m[4] * o.y + m[8]  * o.z + m[12];
	r.y = m[1] * o.x + m[5] * o.y + m[9]  * o.z + m[13];
	r.z = m[2] * o.x + m[6] * o.y + m[10] * o.z + m[14];
	return r;
}


/* create unrolled multiplication 

stringstream ss;		
for(unsigned i=0; i<4; ++i) {
	for(unsigned j=0; j<4; ++j) {
		ss << "r.m[" << (i+j*4) << "] = ";
		for(unsigned k=0; k<4; ++k) {
			ss << " m[" << (i+k*4) << "] * o.m[" <<(k+j*4) << "]";
			if(k != 3) {
				ss << " + " ;
			}
		}
		ss << ";";
		ss << endl;
		ss << endl;
	}
}
cout << ss.str() << endl;
*/
#endif