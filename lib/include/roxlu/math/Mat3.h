#ifndef ROXLU_MAT3H
#define ROXLU_MAT3H

/*
 
  Using column major:
 
  0  3  6
  1  4  7
  2  5  8

*/

#include <roxlu/math/Mat4.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/core/Constants.h>

using std::ostream;
using std::setw;
using std::endl; 
using std::cout; 

namespace roxlu {

  struct Mat3 {

    Mat3(){
      m[0] = 1.0f;
      m[1] = 0.0f;
      m[2] = 0.0f;
		
      m[3] = 0.0f;
      m[4] = 1.0f;
      m[5] = 0.0f;
		
      m[6] = 0.0f;
      m[7] = 0.0f;
      m[8] = 1.0f;
    }
	
    Mat3(
         float m00, float m01, float m02
         ,float m03, float m04, float m05
         ,float m06, float m07, float m08
         );
	
    Mat3(const Vec3& axX, const Vec3& axY, const Vec3& axZ);
	
    // Copy constructor.
    Mat3(const Mat3& o); 
    Mat3(const Mat4& o);
	
    // Assignment operators
    Mat3& operator=(const Mat3& o);
    Mat3& operator=(const Mat4& o);
    Mat3& operator+=(const Mat3& o);
    Mat3& operator-=(const Mat3& o);
    Mat3& operator*=(const Mat3& o);
    Mat3& operator*=(const float s);
	
    inline float& operator()(unsigned int row, unsigned int col);
    inline float operator()(unsigned int row, unsigned int col) const;
    float& operator[](const unsigned int dx);

    //inline float* getPtr();
	
    float* getPtr() { return &m[0]; }
    const float* getPtr() const { return &m[0]; }
	
    // Comparison
    bool operator==(const Mat3& o) const;
    bool operator!=(const Mat3& o) const;
	
    bool isZero() const;
    bool isIdentity() const;
	
    // Get rows and/or columns
    void setRows(const Vec3& row0, const Vec3& row1, const Vec3& row2);
    void getRows(Vec3& row0, Vec3& row1, Vec3& row2);
    Vec3 getRow(unsigned int i);
    void setColumns(const Vec3& col0, const Vec3& col1, const Vec3& col2);
    void getColumns(Vec3& col0, Vec3& col1, Vec3& col2);
    Vec3 getColumn(unsigned int i);
	
    // Matric types
    void identity();
    Mat3& inverse();
    Mat3& transpose();
    static Mat3 rotation(float angleX, float angleY, float angleZ); // 2012.09, angles in radians (was in degrees)
    static Mat3 rotation(float angle, float x, float y, float z);
    static Mat3 rotationX(float angle);
    static Mat3 rotationY(float angle);
    static Mat3 rotationZ(float angle);
		
    // multiplication
    Mat3 operator*(const Mat3& o) const;
    Mat3 operator*(const float s) const;
    Vec3 operator*(const Vec3& v) const; // column multiplication
    friend Vec3 operator*(const Vec3& v, const Mat3& m); // row multiplication
    friend Mat3	operator*(const float s, const Mat3& o);
	
    // addition and subtraction
    Mat3 operator+(const Mat3& o) const;
    Mat3 operator-(const Mat3& o) const;
    Mat3 operator-();
	
    // handy stuff

    //void makeCoordinateSystemPBRT(const Vec3& z); // make coordinate system from one direction vector (x/y axis may flip) from Physically Based Rendering (pbrt)
    void makeCoordinateSystem(const Vec3& z); // Jeppe Revall Frisvad implementation, Fast orthonormal basis (2012): http://www2.imm.dtu.dk/~jrf/papers/abstracts/onb.html
    void makeCoordinateSystem(const Vec3& vz, const Vec3& up);  // make coordinate system from direction vector + up
    Vec3 getXAxis() const;
    Vec3 getYAxis() const;
    Vec3 getZAxis() const;
    static Mat3 getLookAtMatrix(const Vec3& eye, const Vec3& lookAtPoint, const Vec3& upVec = Vec3(0,1,0));
    float trace() const;
    inline void print() const;
    inline friend ostream& operator<<(ostream& os, const Mat3& m);
    float m[9];
  };

  inline float& Mat3::operator()(unsigned int row, unsigned int col) {
    return m[col * 3 + row];
  }

  inline float Mat3::operator()(unsigned int row, unsigned int col) const {
    return m[col * 3 + row];
  }

  //inline float* Mat3::getPtr() {
  //	return &m[0];
  //}

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


  inline Mat3::Mat3(
                    float m00, float m01, float m02
                    ,float m03, float m04, float m05
                    ,float m06, float m07, float m08
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
  }

  inline Mat3::Mat3(const Vec3& axX, const Vec3& axY, const Vec3& axZ) {
    m[0] = axX.x;
    m[1] = axX.y;
    m[2] = axX.z;
	
    m[3] = axY.x;
    m[4] = axY.y;
    m[5] = axY.z;
	
    m[6] = axZ.x;
    m[7] = axZ.x;
    m[8] = axZ.x;
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

  inline float& Mat3::operator[](const unsigned int dx) {      
    return m[dx];
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

  inline float Mat3::trace() const {
    return m[0] + m[4] + m[8];
  }

  inline void Mat3::print() const {
    printf("%3.3f, %3.3f, %3.3f\n", m[0], m[3], m[6]);
    printf("%3.3f, %3.3f, %3.3f\n", m[1], m[4], m[7]);
    printf("%3.3f, %3.3f, %3.3f\n", m[2], m[5], m[8]);

    printf("\n");
  }

} // roxlu
#endif
