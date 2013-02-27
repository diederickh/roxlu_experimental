#ifndef ROXLU_VEC2H
#define ROXLU_VEC2H

// If you're smarter then the optimizer!
//------------------------------------------------------------------------------
#define roxlu_dot2(a,b,r) 			r = (a.x * b.x) + (a.y * b.y);

#define roxlu_isqrt2(a,odist)	  	odist=(float)(a.x*a.x+a.y*a.y); \
  float rsf=odist*0.5f;                                                 \
  long rsl=*(long*)&odist;                                              \
  const float rsopf = 1.5f;                                             \
  rsl=0x5f3759df-(rsl>>1);                                              \
  odist=*(float*)&rsl;                                                  \
  odist=odist*(rsopf-(rsf*odist*odist));  
#define roxlu_copy2(a,b)			b.x = a.x; b.y = a.y;;					
#define roxlu_set2(a,x,y)			a.x = x; a.y = y;;
#define roxlu_normalize2(a,l,b)		l = 0.0;        \
  roxlu_isqrt2(a,l);                                    \
  b.x = a.x * l;                                        \
  b.y = a.y * l;
#define	roxlu_subtract2(a,b,c)		c.x = a.x - b.x; c.y = a.y - b.y;
#define	roxlu_add2(a,b,c)			c.x = a.x + b.x; c.y = a.y + b.y;;
#define roxlu_multiply2(a,b,c)		c.x = a.x * b; c.y = a.y * b;
#define roxlu_zero2(a)				a.x = 0; a.y = 0; 
#define roxlu_divide2(a,f,r)		r.x = a.x / f; r.y = a.y / f; 

#include <iostream>
#include <math.h>
#include <stdio.h>

namespace roxlu { 

  struct Vec3;

  struct Vec2 {
    Vec2();
    Vec2(const float xx, const float yy);
    Vec2(const float n);
    Vec2(const Vec2& v);
    Vec2(const Vec3& v);
    //Vec2(const Vec3& v, int dropAxis); // makes an Vec2(int,int) ambiguous and we never use this
	
	
    // Basic vector math.
    void set(float x, float y);
    float length() const;
    float lengthSquared();
    Vec2& normalize();
    Vec2 getNormalized() const;
	
    const float* getPtr() { return &x; }
	
    // Accessors
    float& operator[](unsigned int index) {
      return (&x)[index];
    }
	
    float operator[](unsigned int index) const {
      return (&x)[index];
    }
	
    // Comparison
    bool operator==(const Vec2& v) const; // v1 == v2 ?
    bool operator!=(const Vec2& v) const; // v2 != v2 ? 
	
    // Assignment operators
    Vec2& operator=(const float& scalar); // v1 = 50.0f;
    Vec2& operator=(const Vec2& v);  // v1 = v2
    Vec2& operator+=(const Vec2& v); // v1 += v2
    Vec2& operator-=(const Vec2& v); // v1 -= v2
	
    Vec2& operator*=(const float s); // v1 *= s
    Vec2& operator/=(const float s); // v1 /= s
    Vec2& operator+=(const float s); // v1 += s

	
    // Other Operators
    Vec2 operator-(); // -v1		
    Vec2 operator-(const Vec2& v) const; // v1 - v2
    Vec2 operator+(const Vec2& v) const; // v1 + v2	Vec2 operator*(const Vec2& v) const;
    Vec2 operator*(const float s) const; // v * 3.0
    Vec2 operator/(const float s) const; // v / 3.0
			
    // Streams
    friend std::ostream& operator<<(std::ostream& os, const Vec2& v);		
    friend std::istream& operator>>(std::istream& is, Vec2& v);		
    void print();
	
    float x;
    float y;
  };

  // -----------------------------------------------------------------------------
  inline void Vec2::set(float xx, float yy) {
    x = xx; 
    y = yy;
  }

  inline float Vec2::length() const{
    return sqrt(x*x + y*y);
  }

  inline float Vec2::lengthSquared() {
    return x*x + y*y;
  }


  inline Vec2& Vec2::normalize() {
    float l = length();
    x/=l;
    y/=l;
    return *this;
  }

  inline Vec2 Vec2::getNormalized() const {
    float l = length();
    float il = 0;
    if(l > 0) {
      il = 1/l;
    }
    return Vec2(x*il, y*il);
  }


  // -----------------------------------------------------------------------------
  inline Vec2& Vec2::operator=(const float& scalar) {
    x = scalar;
    y = scalar;
    return *this;
  }

  inline Vec2& Vec2::operator=(const Vec2& v) {
    x = v.x;
    y = v.y;
    return *this;
  }

  inline Vec2& Vec2::operator-=(const Vec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  inline Vec2& Vec2::operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }


  inline Vec2& Vec2::operator*=(const float s) {
    x *= s;
    y *= s;
    return *this;
  }

  /*
    inline Vec2 Vec2::operator*(const Vec2& v) const {
    return Vec2(v.x * x, v.y * y);
    }
  */

  inline Vec2& Vec2::operator/=(const float s) {
    float inv = 1.0f/s;
    x *= inv;
    y *= inv;
    return *this;
  }

  inline bool Vec2::operator==(const Vec2& v) const {
    return x == v.x && y == v.y;
  }

  inline bool Vec2::operator!=(const Vec2& v) const {
    return x != v.x || y != v.y;
  }

  inline Vec2& Vec2::operator+=(const float s) {
    x += s;
    y += s;
    return *this;
  }

  // -----------------------------------------------------------------------------
  inline Vec2 Vec2::operator-() {
    return Vec2(-x, -y);
  }

  inline Vec2 Vec2::operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
  }

  inline Vec2 Vec2::operator+(const Vec2& v) const {
    return Vec2(x+v.x, y+v.y);
  }


  inline Vec2 Vec2::operator*(const float s) const {
    return Vec2(x*s, y*s);
  }



  inline Vec2 Vec2::operator/(const float s) const {
    float inv = 1.0f/s;
    return Vec2(x*inv, y*inv);
  }

  inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    os << v.x << ", " << v.y;
    return os;
  }

  inline std::istream& operator>>(std::istream& is, Vec2& v) {
    is >> v.x;
    is.ignore(2);
    is >> v.y;
    return is;
  }

  // non members
  // -----------------------------------------------------------------------------
  // for: float * Vec2 (<-- is something else then: Vec2 * float)
  inline Vec2 operator*(const float& s, const Vec2& v) {
    return Vec2(v.x * s, v.y * s);
  }

  inline Vec2 operator-(const float& f, const Vec2& v) {
    return Vec2(f-v.x, f-v.y);
  }

  inline Vec2 operator/(const float& f, const Vec2& v) {
    return Vec2(f/v.x, f/v.y);
  }

  inline Vec2 operator+(const float& f, const Vec2& v) {
    return Vec2(f+v.x, f+v.y);
  }

  // -----------------------------------------------------------------------------
  inline void Vec2::print() {
    printf("%f, %f\n", x, y);
  }

} // roxlu

#endif
