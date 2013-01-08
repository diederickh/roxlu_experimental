#ifndef ROXLU_QAUTH
#define ROXLU_QAUTH

#include <roxlu/math/Mat4.h>
#include <roxlu/math/Mat3.h>
#include <roxlu/math/Vec3.h>
#include <roxlu/core/Constants.h>
#include <roxlu/core/Utils.h>


/* 

Quaternion class, handy tool for 3D rotations.
----------------------------------------------
- When using quaternions for rotations you need to use 
  unit quaternions! This is super importants as some functions
  are optimized for this!
- Slerp should not exceed PI!, PI * 0.99 works, PI not. And slerp should 
  be used to interpolate small values 

Resources
---------:
[0]   Intel, http://software.intel.com/sites/default/files/m/d/4/1/d/8/293748.pdf
      "From Quaternion to Matrix and Back", J.M.P. van Waveren, 27th feb. 2005, id software
[1]   Irrlicht: http://irrlicht.sourceforge.net/
[2]   Quaternions, Ken Shoemake, ftp://ftp.cis.upenn.edu/pub/graphics/shoemake/quatut.ps.Z
[3]   Game Engine Physics Development, Ian Millington, https://github.com/idmillington/cyclone-physics/tree/master/src
[4]   GamePlay, http://www.gameplay3d.org
[5]   Essential mathematics for games and interactive applications, Bishop
[6]   Slerping Clock Cycles, J.M.P van Waveren,  http://software.intel.com/sites/default/files/m/d/4/1/d/8/293747_293747.pdf
[7]   Slerp from GamePlay, https://raw.github.com/blackberry/GamePlay/master/gameplay/src/Quaternion.cpp

*/


namespace roxlu {
  static float sin_zero_half_pi(float a);
  static float atan_positive(float y, float x);
  
  class Quat {
  public:	
    float x;
    float y;
    float z;
    float w;

    Quat(float x = 0, float y = 0, float z = 0, float w = 1);
    Quat(Mat4& m);
    Quat(Mat3& m);
    Quat(const Quat& q);

    void set(const float xx, const float yy, const float zz, const float ww);
    void normalize();
    float length();
    float lengthSquared();
    float dot(Quat& other);
    void identity();
    void inverse();
    void toMat4(Mat4& m);
    void toMat3(Mat3& m);
    Mat3 getMat3();
    Mat4 getMat4();
    void fromMat4(Mat4& m);
    void fromMat3(Mat3& m);
    void fromAngleAxis(const float radians, const float xx, const float yy, const float zz); 
    void fromEuler(const float radiansX, const float radiansY, const float radiansZ);
    void multiply(const Quat& q);
    void multiply(const Quat& q1, const Quat& q2, Quat* dst);
    Vec3 transform(const Vec3& v) const;
    void lerp(const Quat& q1, const Quat& q2, float t, Quat* dst);
    static void slerp(const Quat& from, const Quat& to, float t, Quat& result);
    void print();

    Vec3 operator*(const Vec3& v) const;
    Quat operator*(const Quat& other) const;
    Quat& operator*=(const Quat& other);
  };

  inline Quat::Quat(Mat4& m) {
    fromMat4(m);
  }

  inline Quat::Quat(Mat3& m) {
    fromMat3(m);
  }

  inline Quat::Quat(const Quat& q) 
    :x(q.x)
    ,y(q.y)
    ,z(q.z)
    ,w(q.w)
  {
    
  }

  inline void Quat::set(const float xx, const float yy, const float zz, const float ww) {
    x = xx;
    y = yy; 
    z = zz;
    w = ww;
  }

  inline void Quat::normalize() {
    float n = x * x + y * y + z * z + w * w;
    if(n == 1.0f) {
      return;
    }
    
    n = sqrt(n);
    if(n < 0.000001f) {
      return;
    }

    n = 1.0 / n;
    x *= n;
    y *= n;
    z *= n;
    w *= n;
  }

  inline float Quat::length() {
    return sqrt(lengthSquared());
  }

  inline float Quat::lengthSquared() {
    return (x * x) + (y * y) + (z * z) + (w * w);
  }

  inline float Quat::dot(Quat& other) {
    return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
  }

  inline void Quat::identity() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
  }

  inline void Quat::inverse() {
    float n = x * x + y * y + z * z + w * w;
    if(n == 1.0f) {
      x = -x;
      y = -y;
      z = -z;
      return;
    }

    if(n < 0.000001f) {
      return;
    }
    
    n = 1.0f / n;
    x = (-x * n);
    y = (-y * n);
    z = (-z * n);
    w = w * n;
  }

  // @see [0], the { } sections are hints for compiler optimization
  inline void Quat::toMat4(Mat4& m) {
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    {
      float xx2 = x * x2;
      float yy2 = y * y2;
      float zz2 = z * z2;

      m[0] = 1.0f - yy2 - zz2;
      m[5] = 1.0f - xx2 - zz2;
      m[10] = 1.0f - xx2 - yy2;
    }
    {
      float yz2 = y * z2;
      float wx2 = w * x2;

      m[6] = yz2 - wx2;
      m[9] = yz2 + wx2;
    }
    {
      float xy2 = x * y2;
      float wz2 = w * z2;
      
      m[1] = xy2 - wz2;
      m[4] = xy2 + wz2;
    }
    {
      float xz2 = x * z2;
      float wy2 = w * y2;
      
      m[8] = xz2 - wy2;
      m[2] = xz2 + wy2;
    }
  }

  inline Mat4 Quat::getMat4() {
    Mat4 m;
    toMat4(m);
    return m;
  }

  // @see [0], the { } sections are hints for compiler optimization
  inline void Quat::toMat3(Mat3& m) {
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;

    {
      float xx2 = x * x2;
      float yy2 = y * y2;
      float zz2 = z * z2;

      m[0] = 1.0f - yy2 - zz2;
      m[4] = 1.0f - xx2 - zz2;
      m[8] = 1.0f - xx2 - yy2;
    }
    {
      float yz2 = y * z2;
      float wx2 = w * x2;

      m[5] = yz2 - wx2;
      m[7] = yz2 + wx2;
    }
    {
      float xy2 = x * y2;
      float wz2 = w * z2;
      
      m[1] = xy2 - wz2;
      m[3] = xy2 + wz2;
    }
    {
      float xz2 = x * z2;
      float wy2 = w * y2;
      
      m[6] = xz2 - wy2;
      m[2] = xz2 + wy2;
    }
  }
  
  inline Mat3 Quat::getMat3() {
    Mat3 m;
    toMat3(m);
    return m;
  }

  // @see [0]
  inline void Quat::fromMat4(Mat4& m) {
    if(m[0] + m[5] + m[10] > 0.0f) {
      float t = +m[0] + m[5] + m[10] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      w = s * t;
      z = (m[4] - m[1]) * s;
      y = (m[2] - m[8]) * s;
      x = (m[9] - m[6]) * s;
    }
    else if(m[0] > m[5] && m[0] > m[10]) {
      float t = +m[0] - m[5] - m[10] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      x = s * t;
      y = (m[4] + m[1]) * s;
      z = (m[2] + m[8]) * s;
      w = (m[9] - m[6]) * s;
    }
    else if(m[5] > m[10]) {
      float t = -m[0] + m[5] - m[10] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      y = s * t;
      x = (m[4] + m[1]) * s;
      w = (m[2] - m[8]) * s;
      z = (m[9] + m[6]) * s;
    }
    else {
      float t = -m[0] - m[5] + m[10] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      z = s * t;
      w = (m[4] - m[1]) * s;
      x = (m[2] + m[8]) * s;
      y = (m[9] + m[6]) * s;
    }
  }

  // @see [0]
  inline void Quat::fromMat3(Mat3& m) {
    if(m[0] + m[4] + m[8] > 0.0f) {
      float t = +m[0] + m[4] + m[8] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      w = s * t;
      z = (m[3] - m[1]) * s;
      y = (m[2] - m[6]) * s;
      x = (m[7] - m[5]) * s;
    }
    else if(m[0] > m[4] && m[0] > m[8]) {
      float t = +m[0] - m[4] - m[8] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      x = s * t;
      y = (m[3] + m[1]) * s;
      z = (m[2] + m[6]) * s;
      w = (m[7] - m[5]) * s;
    }
    else if(m[4] > m[8]) {
      float t = -m[0] + m[4] - m[8] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      y = s * t;
      x = (m[3] + m[1]) * s;
      w = (m[2] - m[6]) * s;
      z = (m[7] + m[5]) * s;
    }
    else {
      float t = -m[0] - m[4] + m[9] + 1.0f;
      float s = rx_fast_sqrt(t) * 0.5f;
      z = s * t;
      w = (m[3] - m[1]) * s;
      x = (m[2] + m[6]) * s;
      y = (m[7] + m[5]) * s;
    }
  }

  inline void Quat::fromAngleAxis(const float radians, const float xx, const float yy, const float zz) {
    const float ha = 0.5f * radians;
    const float s = sin(ha);
    w = cos(ha);
    x = s * xx;
    y = s * yy;
    z = s * zz;
  }

  inline void Quat::fromEuler(const float radiansX, const float radiansY, const float radiansZ) {

    // Assuming the angles are in radians.
    float c1 = cos(radiansY);
    float s1 = sin(radiansY);
    float c2 = cos(radiansZ);
    float s2 = sin(radiansZ);
    float c3 = cos(radiansX);
    float s3 = sin(radiansX);
    w = sqrt(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0;
    float w4 = (4.0 * w);
    x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4 ;
    y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4 ;
    z = (-s1 * s3 + c1 * s2 * c3 +s2) / w4 ;
  }

  inline void Quat::multiply(const Quat& q) {
    multiply(*this, q, this);
  }
  
  inline void Quat::multiply(const Quat& q1, const Quat& q2, Quat* dst) {
    float xx = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    float yy = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    float zz = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    float ww = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

    dst->x = xx;
    dst->y = yy;
    dst->z = zz;
    dst->w = ww;
  }

  inline Vec3 Quat::transform(const Vec3& v) const {
    float m = (2.0f * x * v.x + y * v.y + z * v.z);
    float c = 2.0f * w;
    float p = c * w - 1.0f;
    return Vec3(
		 p * v.x + m * x + c * (y * v.z - z * v.y)
		,p * v.y + m * y + c * (z * v.x - x * v.z)
		,p * v.z + m * z + c * (x * v.y - y * v.x)
                );

  }

  inline void Quat::lerp(const Quat& q1, const Quat& q2, float t, Quat* dst) {
    if(t == 0.0f) {
      memcpy(&dst->x, &q1.x, sizeof(float) * 4);
      return;
    }
    else if(t == 1.0f) {
      memcpy(&dst->x, &q2.x, sizeof(float) * 4);
      return;
    }

    float t1 = 1.0f - t;
    dst->x = t1 * q1.x + t * q2.x;
    dst->y = t1 * q1.y + t * q2.y;
    dst->z = t1 * q1.z + t * q2.z;
    dst->w = t1 * q1.w + t * q2.w;
  }

  float sin_zero_half_pi(float a) {
    float s, t; 
    s = a * a; 
    t = -2.39e-08f; 
    t *= s; 
    t += 2.7526e-06f; 
    t *= s; 
    t += -1.98409e-04f; 
    t *= s; 
    t += 8.3333315e-03f; 
    t *= s; 
    t += -1.666666664e-01f; 
    t *= s; 
    t += 1.0f; 
    t *= a; 
    return t; 
  }

  float atan_positive(float y, float x) {
    float a, d, s, t; 
    if( y > x ) { 
      a = -x / y; 
      d = PI / 2; 
    } 
    else { 
      a = y / x; 
      d = 0.0f; 
    } 
    s = a * a; 
    t = 0.0028662257f; 
    t *= s;     
    t += -0.0161657367f; 
    t *= s; 
    t += 0.0429096138f; 
    t *= s; 
    t += -0.0752896400f; 
    t *= s; 
    t += 0.1065626393f; 
    t *= s; 
    t += -0.1420889944f; 
    t *= s; 
    t += 0.1999355085f; 
    t *= s; 
    t += -0.3333314528f; 
    t *= s; 
    t += 1.0f; 
    t *= a; 
    t += d; 
    return t; 
  }
  
  inline void Quat::slerp(const Quat& from, const Quat& to, float t, Quat& result) {
    if(t <= 0.0f) {
      memcpy(&result.x, &from.x, sizeof(float) * 4);
      return;
    }
    else if(t >= 1.0f) {
      memcpy(&result.x, &to.x, sizeof(float) * 4);
      return;
    }
    
    float cosom, abs_cosom, sinom, sin_sqr, omega, scale0, scale1; 
    cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w; 
    abs_cosom = fabs( cosom ); 
    if ( ( 1.0f - abs_cosom ) > 1e-6f ) { 
      sin_sqr = 1.0f - abs_cosom * abs_cosom; 
      sinom = rx_fast_sqrt( sin_sqr ); 
      omega = atan_positive( sin_sqr * sinom, abs_cosom ); 
      scale0 = sin_zero_half_pi( ( 1.0f - t ) * omega ) * sinom; 
      scale1 = sin_zero_half_pi( t * omega ) * sinom; 
    } else { 
      scale0 = 1.0f - t; 
      scale1 = t; 
    } 
    scale1 = ( cosom >= 0.0f ) ? scale1 : -scale1; 
    result.x = scale0 * from.x + scale1 * to.x; 
    result.y = scale0 * from.y + scale1 * to.y; 
    result.z = scale0 * from.z + scale1 * to.z; 
    result.w = scale0 * from.w + scale1 * to.w;
  }

  inline Quat Quat::operator*(const Quat& other) const {
    Quat result(*this);
    result.multiply(other);
    return result;
  }

  inline Vec3 Quat::operator*(const Vec3& v) const {
    return transform(v);
  }

  inline Quat& Quat::operator*=(const Quat& other) {
    multiply(other);
    return *this;
  }

  inline void Quat::print() {
    printf("%f, %f, %f, %f\n", x, y, z, w);
  }

} // roxlu
#endif
