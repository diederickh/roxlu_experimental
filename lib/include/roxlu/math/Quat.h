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

Resources:
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

    void set(Vec3 axis, float radians);
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
    void multiply(const Quat& q);
    void multiply(const Quat& q1, const Quat& q1, Quat* dst);
    Vec3 transform(const Vec3& v) const;
    void lerp(const Quat& q1, const Quat& q1, float t, Quat* dst);
    void slerp(const Quat& q1, const Quat& q2, float t, Quat* dst);
    void slerp(float q1x, float q1y, float q1z, 
               float q1w, float q2x, float q2y, 
               float q2z, float q2w, 
               float t, 
               float* dstx, float* dsty, float* dstz, float* dstw
               );
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

  inline void Quat::set(Vec3 axis, float radians) {
    float a = radians * 0.5;
    float s = sin(a);

    axis.normalize();

    x = axis.x * s;
    y = axis.y * s;
    z = axis.z * s;
    w = cos(a);
  }

  inline void Quat::normalize() {
    float n = x * x + y * y + z * z + w * w;
    if(n == 1.0f) {
      printf("Quat::normalize(), no need to normalize!\n");
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
      memcpy(&dst->x, &q1, sizeof(float) * 4);
      return;
    }
    else if(t == 1.0f) {
      memcpy(&dst->x, &q2, sizeof(float) * 4);
      return;
    }

    float t1 = 1.0f - t;
    dst->x = t1 * q1.x + t * q2.x;
    dst->y = t1 * q1.y + t * q2.y;
    dst->z = t1 * q1.z + t * q2.z;
    dst->w = t1 * q1.w + t * q2.w;
  }

  inline void Quat::slerp(const Quat& q1, const Quat& q2, float t, Quat* dst) {
    slerp(q1.x, q1.y, q1.z, q1.w, q2.x, q2.y, q2.z, q2.w, t, &dst->x, &dst->y, &dst->z, &dst->w);
  }

  // @see [7]
  inline void Quat::slerp(float q1x, float q1y, float q1z, 
                          float q1w, float q2x, float q2y, 
                          float q2z, float q2w, 
                          float t, 
                          float* dstx, float* dsty, float* dstz, float* dstw
                          )
  {
    // Fast slerp implementation by kwhatmough:
    // It contains no division operations, no trig, no inverse trig
    // and no sqrt. Not only does this code tolerate small constraint
    // errors in the input quaternions, it actually corrects for them.

    if(t == 0.0f) {
      *dstx = q1x;
      *dsty = q1y;
      *dstz = q1z;
      *dstw = q1w;
      return;
    }
    else if(t == 1.0f)  {
      *dstx = q2x;
      *dsty = q2y;
      *dstz = q2z;
      *dstw = q2w;
      return;
    }

    if(q1x == q2x && q1y == q2y && q1z == q2z && q1w == q2w) {
      *dstx = q1x;
      *dsty = q1y;
      *dstz = q1z;
      *dstw = q1w;
      return;
    }

    float half_y, alpha, beta;
    float u, f1, f2a, f2b;
    float ratio1, ratio2;
    float half_sec_half_theta, vers_half_theta;
    float sq_not_u, sq_u;

    float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

    // As usual in all slerp implementations, we fold theta.
    alpha = cosTheta >= 0 ? 1.0f : -1.0f;
    half_y = 1.0f + alpha * cosTheta;

    // Here we bisect the interval, so we need to fold t as well.
    f2b = t - 0.5f;
    u = f2b >= 0 ? f2b : -f2b;
    f2a = u - f2b;
    f2b += u;
    u += u;
    f1 = 1.0f - u;

    // One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
    half_sec_half_theta = 1.09f - (0.476537f - 0.0903321f * half_y) * half_y;
    half_sec_half_theta *= 1.5f - half_y * half_sec_half_theta * half_sec_half_theta;
    vers_half_theta = 1.0f - half_y * half_sec_half_theta;

    // Evaluate series expansions of the coefficients.
    sq_not_u = f1 * f1;
    ratio2 = 0.0000440917108f * vers_half_theta;
    ratio1 = -0.00158730159f + (sq_not_u - 16.0f) * ratio2;
    ratio1 = 0.0333333333f + ratio1 * (sq_not_u - 9.0f) * vers_half_theta;
    ratio1 = -0.333333333f + ratio1 * (sq_not_u - 4.0f) * vers_half_theta;
    ratio1 = 1.0f + ratio1 * (sq_not_u - 1.0f) * vers_half_theta;

    sq_u = u * u;
    ratio2 = -0.00158730159f + (sq_u - 16.0f) * ratio2;
    ratio2 = 0.0333333333f + ratio2 * (sq_u - 9.0f) * vers_half_theta;
    ratio2 = -0.333333333f + ratio2 * (sq_u - 4.0f) * vers_half_theta;
    ratio2 = 1.0f + ratio2 * (sq_u - 1.0f) * vers_half_theta;

    // Perform the bisection and resolve the folding done earlier.
    f1 *= ratio1 * half_sec_half_theta;
    f2a *= ratio2;
    f2b *= ratio2;
    alpha *= f1 + f2a;
    beta = f1 + f2b;

    // Apply final coefficients to a and b as usual.
    float w = alpha * q1w + beta * q2w;
    float x = alpha * q1x + beta * q2x;
    float y = alpha * q1y + beta * q2y;
    float z = alpha * q1z + beta * q2z;

    // This final adjustment to the quaternion's length corrects for
    // any small constraint error in the inputs q1 and q2 But as you
    // can see, it comes at the cost of 9 additional multiplication
    // operations. If this error-correcting feature is not required,
    // the following code may be removed.
    f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
    *dstw = w * f1;
    *dstx = x * f1;
    *dsty = y * f1;
    *dstz = z * f1;
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

} // roxlu
#endif
