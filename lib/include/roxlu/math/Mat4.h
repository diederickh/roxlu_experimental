#ifndef ROXLU_MAT4H
#define ROXLU_MAT4H

#include <iostream>
#include <iomanip>
#include <cstring> // memcpy
#include <stdio.h>
#include <roxlu/math/Vec4.h>
#include <roxlu/math/Vec3.h>

// see here: https://github.com/evanw/gl4/blob/master/gl4.cpp
// 2012.09, rotation is in RADIANS! (was in degrees)
using std::ostream;

namespace roxlu {

  struct Mat3;

  struct Mat4 {

    inline Mat4();

    // pass all values to the c'tor. numbers indicate array index
    Mat4(
         float m00, float m01, float m02, float m03
         ,float m04, float m05, float m06, float m07
         ,float m08, float m09, float m10, float m11
         ,float m12, float m13, float m14, float m15
         );
	
    Mat4(Vec3 axX, Vec3 axY, Vec3 axZ, Vec3 pos = Vec3(0,0,0)); 
    Mat4(const Mat3& o);
	
    // copy
    Mat4(const Mat4& o);
    Mat4& operator=(const Mat4& o);
    bool operator==(const Mat4& o) const;
    bool operator!=(const Mat4& o) const;
	
    bool isZero();
    bool isIdentity();
    void clean();
    void identity();
    void copyTo(float* f);
    void copyFrom(float* f);

		
    // @todo maybe we should not return anything here (?)	
    void affineInverse();	 
    void inverse();
    void transpose();
    void translate(const Vec3& v);
    void translate(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);

    void scale(float s);
    void scale(float x, float y, float z);
	
    void setPosition(float x, float y, float z);
    void setPosition(const Vec3& pos);
    void setX(const float& x);
    void setY(const float& y);
    void setZ(const float& z);
    void setScale(const float x, const float y, const float z);
    void setScale(const float s);
    void setZRotation(const float radians);
    void setYRotation(const float radians);
    void setXRotation(const float radians);

    float getX();
    float getY();
    float getZ();
    Vec3 getPosition();
	
    void setBillboard(const Vec3& right, const Vec3& up); // create billboard matrix (z-axis is calculated)
    void setBillboard(const float* right, const float* up);
    void setAxis(const Vec3& x, const Vec3& y, const Vec3& z);
    void setXAxis(const Vec3& ax); // indices: 0,1,2
    void setXAxis(const float* ax); 
    void setYAxis(const Vec3& ax); // indices: 4,5,6
    void setYAxis(const float* ax); 
    void setZAxis(const Vec3& ax); // indices: 8,9,10
    void setZAxis(const float* ax);
	
    static Mat4 translation(const Vec3& v);
    static Mat4 translation(const float x, const float y, const float z);
    static Mat4 rotation(const Mat3& o);
    static Mat4 rotation(float angleX, float angleY, float angeZ);
    static Mat4 rotation(float a, float x, float y, float z);
    static Mat4 rotationX(float a); // rotate around x-axis
    static Mat4 rotationY(float a); // rotate around y-axis
    static Mat4 rotationZ(float a); // rotate around z-axis
    static Mat4 scaling(float x, float y, float z);
	
    Mat4& frustum(float l, float r, float b, float t, float n, float f);
    Mat4& frustumCenter(float w, float h, float n, float f);
    Mat4& perspective(float fovDegrees, float aspect, float n, float f);
    Mat4& ortho(float l, float r, float b, float t, float n, float f);
    Mat4& orthoCenter(float w, float h, float n, float f);
    Mat4& orthoTopLeft(float w, float h, float n, float f);
    Mat4& orthoBottomLeft(float w, float h, float n, float f);	
	
    Mat4 getInverse();
    Mat4 getTranspose();
	
    // rotate and translate
    inline Vec3 transform(const Vec3& v) const;

    Mat4& operator+=(const Mat4& o);
    Mat4& operator-=(const Mat4& o);
    Mat4& operator*=(const Mat4& o);
    Mat4& operator*=(float s);
    Mat4& operator=(const Mat3& o);
    Mat3 operator=(const Mat4& o) const;
	
    Mat4  operator+(const Mat4& o) const;
    Mat4  operator-(const Mat4& o) const;
    Mat4  operator-() const;
    Mat4  operator*(const Mat4& o) const;	
    Mat4  operator*(float s);
    Vec4  operator*(const Vec4& v) const;
	
	
    float& operator[](const unsigned int);
    float operator[](const unsigned int) const;
	
    inline float* getPtr() { return &m[0]; }
    inline const float* getPtr() const { return &m[0]; }
	
    float m[16];
	
    // stream operator, debug.
    void print() const;
    friend ostream& operator<<(ostream& os, const Mat4& o);
  };

  extern Mat4 operator*(float s, const Mat4& o);
  extern Mat4 affine_inverse(const Mat4& o);
  extern Mat4 transpose(const Mat4& o);
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


  inline Mat4::Mat4(
                    float m00, float m01, float m02, float m03
                    ,float m04, float m05, float m06, float m07
                    ,float m08, float m09, float m10, float m11
                    ,float m12, float m13, float m14, float m15
                    )
  {
    m[0] = m00;
    m[1] = m01;
    m[2] = m02;
    m[3] = m03;
    m[4] = m04;
    m[5] = m05;
    m[6] = m06;
    m[7] = m07;
    m[8] = m08;
    m[9] = m09;
    m[10] = m10;
    m[11] = m11;
    m[12] = m12;
    m[13] = m13;
  }

  inline Mat4::Mat4(Vec3 axX, Vec3 axY, Vec3 axZ, Vec3 pos) {
    m[0] = axX.x;
    m[1] = axX.y;
    m[2] = axX.z;
    m[3] = 0.0f;
	
    m[4] = axY.x;
    m[5] = axY.y;
    m[6] = axY.z;
    m[7] = 0.0f;
	
    m[8] = axZ.x;
    m[9] = axZ.y;
    m[10] = axZ.z;
    m[11] = 0.0f;
	
    m[12] = pos.x;
    m[13] = pos.y;
    m[14] = pos.z;
    m[15] = 1.0f;
  }

  inline void Mat4::print() const {
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", m[0], m[4], m[8], m[12]);
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", m[1], m[5], m[9], m[13]);
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", m[2], m[6], m[10], m[14]);
    printf("%3.3f, %3.3f, %3.3f, %3.3f\n", m[3], m[7], m[11], m[15]);
    printf("\n");
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


  inline void Mat4::setPosition(const Vec3& pos) {
    m[12] = pos.x;
    m[13] = pos.y;
    m[14] = pos.z;
  }

  inline void Mat4::setPosition(float x, float y, float z) {
    m[12] = x;
    m[13] = y; 
    m[14] = z;
  }

  inline float& Mat4::operator[](const unsigned int dx) {
    return m[dx];
  }

  inline float Mat4::operator[](const unsigned int dx) const {
    return m[dx];
  }

  inline void Mat4::copyTo(float* f) {
    memcpy(f,m,16*sizeof(float));
  }

  inline void Mat4::copyFrom(float* f) {
    memcpy(m,f, 16*sizeof(float));
  }

  inline Vec3 Mat4::getPosition() {
    Vec3 v(m[12], m[13], m[14]);
    return v;
  }

  inline void Mat4::setX(const float& x) {
    m[12] = x;
  }

  inline void Mat4::setY(const float& y) {
    m[13] = y;
  }

  inline void Mat4::setZ(const float& z) {
    m[14] = z; 
  }

  inline float Mat4::getX() {
    return m[12];
  }

  inline float Mat4::getY() {
    return m[13];
  }

  inline float Mat4::getZ() {
    return m[14];
  }

  inline void Mat4::setAxis(const Vec3& x, const Vec3& y, const Vec3& z) {
    setXAxis(x);
    setYAxis(y);
    setZAxis(z);
  }

  inline void Mat4::setXAxis(const float* ax) {
    m[0] = ax[0];
    m[1] = ax[1];
    m[2] = ax[2];
  }

  inline void Mat4::setXAxis(const Vec3& ax) { // indices: 0,1,2
    m[0] = ax.x;
    m[1] = ax.y;
    m[2] = ax.z;	
  }

  inline void Mat4::setYAxis(const Vec3& ax) { // indices: 4,5,6
    m[4] = ax.x;
    m[5] = ax.y;
    m[6] = ax.z;	
  }

  inline void Mat4::setYAxis(const float* ax) {
    m[4] = ax[0];
    m[5] = ax[1];
    m[6] = ax[2];
  }

  inline void Mat4::setZAxis(const Vec3& ax) { // indices: 8,9,10
    m[8] = ax.x;
    m[9] = ax.y;
    m[10] = ax.z;	
  }

  inline void Mat4::setZAxis(const float* ax) {
    m[8] = ax[0];
    m[9] = ax[1];
    m[10] = ax[2];
  }

  inline void Mat4::rotateX(float angle) {
    rotate(angle, 1.0f, 0.0f, 0.0f);
  }

  inline void Mat4::rotateY(float angle) {
    rotate(angle, 0.0f, 1.0f, 0.0f);
  }

  inline void Mat4::rotateZ(float angle) {
    rotate(angle, 0.0f, 0.0f, 1.0f);
  }

  // set the billboard vectors (you can retrieve the right and up by using the inverse rotation of the view matrix, see Camera.h)
  inline void Mat4::setBillboard(const Vec3& right, const Vec3& up) {
    setXAxis(right);
    setYAxis(up);
  }

  inline void Mat4::setBillboard(const float* right, const float* up) {
    setXAxis(right);
    setYAxis(up);
  }
	
} // roxlu

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
