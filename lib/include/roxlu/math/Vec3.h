#ifndef ROXLU_VEC3H
#define ROXLU_VEC3H

// When you're smarter than the compiler optimizer
// -----------------------------------------------------------------------------
#define roxlu_dot3(a,b,r) r = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
#define roxlu_cross3(a,b,dest)	dest.x = (a.y * b.z) - (b.y * a.z);     \
  dest.y = (a.z * b.x) - (b.z * a.x);                                   \
  dest.z = (a.x * b.y) - (b.x * a.y);


#define roxlu_isqrt3(a,odist)  {                \
    odist=(float)(a.x*a.x+a.y*a.y + a.z * a.z); \
    float rsf=odist*0.5f;                       \
    long rsl=*(long*)&odist;                    \
    const float rsopf = 1.5f;                   \
    rsl=0x5f3759df-(rsl>>1);                    \
    odist=*(float*)&rsl;                        \
    odist=odist*(rsopf-(rsf*odist*odist));      \
  }
#define roxlu_length3(a,r)			roxlu_isqrt3(a,r);      \
  r = 1/r;
#define roxlu_copy3(a,b)			b.x = a.x; b.y = a.y; b.z = a.z;					
#define roxlu_set3(a,x,y,z)			a.x = x; a.y = y; a.z = z;
#define roxlu_normalize3(a,l,d)		l = 0.0;        \
  roxlu_isqrt3(a,l);                                    \
  d.x = a.x * l;                                        \
  d.y = a.y * l;                                        \
  d.z = a.z * l; 
							

#define roxlu_rotate3(a, angle, ax, result)                             \
  {                                                                     \
    float sina = sin(angle);                                            \
    float cosa = cos(angle);                                            \
    float cosb = 1.0f - cosa;                                           \
    result.x = a.x * (ax.x  * ax.x * cosb + cosa) + a.y * (ax.x * ax.y * cosb - ax.z * sina) + a.z * (ax.x * ax.z * cosb + ax.y * sina); \
    result.y = a.x * (ax.y  * ax.x * cosb + ax.z * sina) + a.y * (ax.y * ax.y * cosb + cosa) + a.z * (ax.y * ax.z * cosb - ax.x * sina); \
    result.z = a.x * (ax.z  * ax.x * cosb - ax.y * sina) + a.y * (ax.z * ax.y * cosb + ax.x * sina) + a.z * (ax.z * ax.z * cosb + cosa); \
  }
	
#define	roxlu_subtract3(a,b,c)		c.x = a.x - b.x; c.y = a.y - b.y; c.z = a.z - b.z;
#define	roxlu_add3(a,b,c)			c.x = a.x + b.x; c.y = a.y + b.y; c.z = a.z + b.z;
#define roxlu_multiply3(a,b,c)		c.x = a.x * b; c.y = a.y * b; c.z = a.z * b;
#define roxlu_zero3(a)				a.x = 0; a.y = 0; a.z = 0;
#define roxlu_divide3(a,f,r)		r.x = a.x / f; r.y = a.y / f; r.z = a.z / f;
#define roxlu_scale3(vec,s,l,r)		roxlu_normalize3(vec,l,vec); r.x = vec.x * s; r.y  = vec.y * s; r.z = vec.z * s;
#define roxlu_ptr3(vec)				&(vec).x

#include <iostream>
#include <math.h>
#include <stdio.h>

namespace roxlu {

  struct Vec2;
  struct Vec3;
  struct Vec4;

  struct Mat3;

  Vec3 cross(const Vec3& a, const Vec3& b);
  float dot(const Vec3& a, const Vec3& b);
  Vec3 rotate(const float angle, const Vec3& v, const Vec3& axis);

  // Vector 3 class.
  // -----------------------------------------------------------------------------
  struct Vec3 {
    friend struct Mat3;
    Vec3(const float* v);
    Vec3(const float xx = 0.0f, const float yy = 0.0f, const float zz = 0.0f);
    Vec3(const Vec2& v);
    Vec3(const Vec3& v);
    Vec3(const Vec4& v);
    Vec3(const float s);

    // Assignment operators
    Vec3& operator=(const float& scalar);
    Vec3& operator=(const Vec4& v4);
    Vec3& operator=(const Vec2& other);
    Vec3& operator-=(const Vec3& other);
    Vec3& operator+=(const Vec3& other);
    Vec3& operator+=(const float scalar);
    Vec3& operator*=(const float scalar);
    Vec3& operator*=(const Vec3& other);
    Vec3& operator/=(const float scalar);

    // Basic methods.	
    Vec3& rotate(float angle, float x, float y, float z);
    Vec3& rotate(float angle, Vec3 axis);
    Vec3& rotate(float angle, Vec3 axis, Vec3 pivot);
    Vec3& rotateY(const float& angle);
    Vec3& rotateX(const float& angle);
    Vec3& rotateZ(const float& angle);
    Vec3& limit(float size);
    Vec3& scale(float length);
    Vec3& normalize();
    Vec3& cross(const Vec3& vec);
    Vec3 getScaled(float length);
    Vec3 getNormalized() const;
    Vec3 getCrossed(const Vec3& vec);
    Vec3 getRotated(const float a, const Vec3& ax) const;

    void set(float n);
    void set(float xx, float yy, float zz);
    float length() const;
    float lengthSquared();
    float lengthSquared(const Vec3& other);
    float dot(const Vec3& vec);
    //	float* getPtr() { return &x; };
    const float* getPtr() { return &x; }
    void print() const;

    // Accessors
    float& operator[](unsigned int index) {
      return (&x)[index];
    }
	
    float operator[](unsigned int index) const {
      return (&x)[index];
    }

    // Comparison
    bool operator==(const Vec3& other) const;
    bool operator!=(const Vec3& other) const;
	
    // Operators
    Vec3 operator-() const; // -v
    Vec3 operator-(const Vec3& other) const; // v1 - v2
    Vec3 operator+(const Vec3& other) const; // v1 + v2
    Vec3 operator*(const float scalar) const; // v1 *= 1.5;
    Vec3 operator/(const float scalar) const; // v1 /= 3.0;
    Vec3 operator^(const Vec3 v) const; // cross
			
    // Streams
    friend std::ostream& operator<<(std::ostream& os, const Vec3& vec);		
    friend std::istream& operator>>(std::istream& is, Vec3& vec);		
			
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

  inline void Vec3::set(float n) {
    x = n;
    y = n;
    z = n;
  }

  inline float Vec3::length() const {
    return sqrt(x*x + y*y + z*z);
  }

  inline Vec3& Vec3::normalize() {
    float l = length();
    x/=l;
    y/=l;
    z/=l;
    return *this;
  }

  inline Vec3 Vec3::getNormalized() const {
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

  inline Vec3& Vec3::rotate(float a, float xx, float yy, float zz) {
    float sina = sin(a);
    float cosa = cos(a);
    float cosb = 1.0f - cosa;
	
    x =  x * (xx * xx * cosb + cosa) + y * (xx * yy * cosb - zz * sina)+ z * (xx * zz * cosb + yy * sina);
    y =  x * (yy * xx * cosb + zz * sina) + y * (yy * yy * cosb + cosa) + z * (yy * zz * cosb - xx * sina);
    z =  x * (zz * xx * cosb - yy * sina) + y * (zz * yy * cosb + xx * sina) + z * (zz * zz * cosb + cosa);
    return *this;
  }

  inline Vec3& Vec3::rotateX(const float& angle) {
    return rotate(angle, 1.0f, 0.0f, 0.0f);
  }

  inline Vec3& Vec3::rotateY(const float& angle) {
    return rotate(angle, 0.0f, 1.0f, 0.0f);
  }

  inline Vec3& Vec3::rotateZ(const float& angle) {
    return rotate(angle, 0.0f, 0.0f, 1.0f);
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

  inline Vec3 Vec3::getRotated(const float a, const Vec3& ax) const {
    Vec3 v = *this;
    v.rotate(a,ax);
    return v;
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

  inline Vec3 Vec3::getScaled(float scale) {
    return Vec3(x*scale, y*scale, z*scale);
  }

  inline Vec3& Vec3::scale(float scale) {
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

  inline Vec3& Vec3::limit(float s) {
    float l = lengthSquared();
    if(l > (s*s)) {
      normalize();	
      *this *= s;
    }
    return *this;
  }

  inline void Vec3::print() const {
    printf("%f, %f, %f\n", x, y, z);
  }

  // -----------------------------------------------------------------------------
  inline bool Vec3::operator==(const Vec3& v) const {
    return x == v.x && y == v.y && z == v.z;
  }

  inline bool Vec3::operator!=(const Vec3& v) const {
    return x != v.x || y != v.y || z != v.z;
  }

  inline Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x-v.x, y-v.y, z-v.z);
  }

  inline Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x+v.x, y+v.y, z+v.z);
  }


  inline Vec3& Vec3::operator=(const float& scalar) {
    x = scalar;
    y = scalar;
    z = scalar;
    return *this;
  }

  inline Vec3& Vec3::operator-=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  inline Vec3& Vec3::operator+=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  inline Vec3& Vec3::operator*=(const Vec3& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
  }

  // Vec3 * float
  inline Vec3 Vec3::operator*(const float scalar) const {
    return Vec3(x*scalar, y*scalar, z*scalar);
  }

  inline Vec3& Vec3::operator*=(const float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }

  inline Vec3& Vec3::operator+=(const float scalar) {
    x += scalar;
    y += scalar;
    z += scalar;
    return *this;
  }

  inline Vec3 Vec3::operator/(const float scalar) const {
    float inv = 1.0f/scalar;
    return Vec3(x*inv, y*inv, z*inv);
  }

  inline Vec3& Vec3::operator/=(const float scalar) {
    float inv = 1.0f/scalar;
    x *= inv;
    y *= inv;
    z *= inv;
    return *this;
  }

  inline Vec3 Vec3::operator-() const{
    Vec3 v ;
    v.x = -x;
    v.y = -y;
    v.z = -z;
    return v;
  }

  // cross (check http://tutorial.math.lamar.edu/Classes/CalcII/CrossProduct.aspx) 
  // read up on the Sarrus rule, or Method of Cofactors, determinant
  inline Vec3 Vec3::operator^(const Vec3 v) const {
    return Vec3(
      y * v.z - z * v.y
      ,z * v.x - x * v.z
      ,x * v.y - y * v.x
    );
  }

  inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
  }

  inline std::istream& operator>>(std::istream& is, Vec3& v) {
    is >> v.x;
    is.ignore(2);
    is >> v.y;
    is.ignore(2);
    is >> v.z;
    return is;
  }

  // non members
  // -----------------------------------------------------------------------------
  inline Vec3 operator+(float f, const Vec3& vec) {
    return Vec3( f+vec.x, f+vec.y, f+vec.z );
  }

  inline Vec3 operator-(float f, const Vec3& vec) {
    return Vec3( f-vec.x, f-vec.y, f-vec.z );
  }

  inline Vec3 operator/(float f, const Vec3& vec) {
    return Vec3( f/vec.x, f/vec.y, f/vec.z);
  }

  inline Vec3 operator*(float f, const Vec3& vec) {
    return Vec3(f*vec.x, f*vec.y, f*vec.z );
  }

  inline Vec3 operator*(const Vec3& a, const Vec3& b) {
    return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
  }

  // -----------------------------------------------------------------------------
  inline Vec3 cross(const Vec3& a, const Vec3& b) {
    Vec3 p;
    roxlu_cross3(a,b,p);
    return p;
  }

  inline float dot(const Vec3& a, const Vec3& b) {
    float r = 0.0;
    roxlu_dot3(a,b,r);
    return r;
  }

  inline Vec3 rotate(const float angle, const Vec3& v, const Vec3& axis) {
    Vec3 result;
    roxlu_rotate3(v, angle, axis, result);
    return result;
    /*
      rotate3(a, angle, ax, result)	\
      { \
      float sina = sin(angle); \
      float cosa = cos(angle); \
      float cosb = 1.0f - cosa; \
      result.x = a.x * (ax.x  * ax.x * cosb + cosa) + a.y * (ax.x * ax.y * cosb - ax.z * sina) + a.z * (ax.x * ax.z * cosb + ax.y * sina);   \
      result.y = a.x * (ax.y  * ax.x * cosb + ax.z * sina) + a.y * (ax.y * ax.y * cosb + cosa) + a.z * (ax.y * ax.z * cosb - ax.x * sina);   \
      result.z = a.x * (ax.z  * ax.x * cosb - ax.y * sina) + a.y * (ax.z * ax.y * cosb + ax.x * sina) + a.z * (ax.z * ax.z * cosb + cosa);   \
      }
    */
  }

} // roxlu

#endif
