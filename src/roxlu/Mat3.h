#ifndef ROXLU_MAT3H
#define ROXLU_MAT3H
/*
 
 Using column major:
 
 0  3  6
 1  4  7
 2  5  8

*/

#include "Vec3.h"
#include <iostream>
#include <iomanip>
#include "ofMain.h"

using std::ostream;
using std::setw;
using std::endl; 
using std::cout; 

class Mat3 {
public:
	enum Flags {
		NONE			= (1 << 0)
		,IDENTITY 		= (1 << 1)
		,TRANSLATION 	= (1 << 2)
		,ROTATION 		= (1 << 3)
		,SCALING		= (1 << 4)
		,MIX			= (1 << 5)
	};

	inline Mat3():flag(NONE){}
	
	// copy constructor.
	Mat3(const Mat3& o); 
	Mat3& operator=(const Mat3& o);
	inline float& operator()(unsigned int row, unsigned int col);
	
	// comparison
	bool operator==(const Mat3& o) const;
	bool operator!=(const Mat3& o) const;
	bool isZero() const;
	bool isIdentity() const;
	
	// get rows and/or columns
	void setRows(const Vec3& row0, const Vec3& row1, const Vec3& row2);
	void getRows(Vec3& row0, Vec3& row1, Vec3& row2);
	Vec3 getRow(unsigned int i);
	void setColumns(const Vec3& col0, const Vec3& col1, const Vec3& col2);
	void getColumns(Vec3& col0, Vec3& col1, Vec3& col2);
	Vec3 getColumn(unsigned int i);
	
	// matric types
	void identity();
	Mat3& inverse();
	inline Mat3& transpose();
	Mat3& rotation(float angleX, float angleY, float angleZ);
	Mat3& rotation(float angle, float x, float y, float z);
	Mat3& rotationX(float angle);
	Mat3& rotationY(float angle);
	Mat3& rotationZ(float angle);
		
	// multiplication
	Mat3 				operator*(const Mat3& o) const;
	Mat3& 				operator*=(const Mat3& o);
	Mat3& 				operator*=(const float s);
	Mat3 				operator*(const float s) const;
	inline Vec3 		operator*(const Vec3& v) const; // column multiplication
	inline friend Vec3 	operator*(const Vec3& v, const Mat3& m); // row multiplication
	friend Mat3			operator*(const float s, const Mat3& o);
	
	// addition and subtraction
	Mat3 			operator+(const Mat3& o) const;
	Mat3& 			operator+=(const Mat3& o);
	Mat3 			operator-(const Mat3& o) const;
	Mat3& 			operator-=(const Mat3& o);
	Mat3 			operator-();
	
	inline friend ostream& operator<<(ostream& os, const Mat3& m);
	float m[9];
	int flag;
};

inline float& Mat3::operator()(unsigned int row, unsigned int col) {
	return m[col * 3 + row];
}

extern Mat3 inverse(Mat3& o);

inline Mat3 transpose(const Mat3& o) {
	Mat3 r;
	r.m[0] = o.m[0];
	r.m[1] = o.m[3];
	r.m[2] = o.m[6];

	r.m[3] = o.m[1];
	r.m[4] = o.m[4];
	r.m[5] = o.m[7];
	
	r.m[6] = o.m[2];
	r.m[7] = o.m[5];	
	r.m[8] = o.m[8];
	return r;
}


// transpose ourself, use transpose(Mat3) to get a copy
inline Mat3& Mat3::transpose() {
	float t = m[1];
	m[1] = m[3];
	m[3] = t;
	
	t    = m[2];
	m[2] = m[6];
	m[6] = t;
	
	t    = m[5];
	m[5] = m[7];
	m[7] = t;
	
	return *this;
}


inline Vec3 Mat3::operator*(const Vec3& v) const {
	Vec3 r;
	r.x = m[0] * v.x + m[3] * v.y + m[6] * v.z;
	r.y = m[1] * v.x + m[4] * v.y + m[7] * v.z;
	r.x = m[2] * v.x + m[5] * v.y + m[8] * v.z;
	return r;
}

inline Vec3 operator*(const Vec3& v, const Mat3& m) {
	Vec3 r;
	r.x = m.m[0] * v.x + m.m[1] * v.y + m.m[2] * v.z;
	r.y = m.m[3] * v.x + m.m[4] * v.y + m.m[5] * v.z;
	r.x = m.m[6] * v.x + m.m[7] * v.y + m.m[8] * v.z;
	return r;
}

inline ostream& operator<<(ostream& os, const Mat3& m) {
	int w = 10;
	os.precision(5);
	os	<< setw(w)
		<< m.m[0] << ", " << setw(w)
		<< m.m[3] << ", " << setw(w) 
		<< m.m[6] << std::endl;

	os	<< setw(w)
		<< m.m[1] << ", " << setw(w)
		<< m.m[4] << ", " << setw(w) 
		<< m.m[7] << std::endl;

	os	<< setw(w)
		<< m.m[2] << ", " << setw(w) 
		<< m.m[5] << ", " << setw(w) 
		<< m.m[8] << std::endl;
	return os;
}
#endif