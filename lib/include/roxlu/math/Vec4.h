#ifndef ROXLU_VEC4H
#define ROXLU_VEC4H

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <roxlu/math/Vec3.h>

using std::setw;

namespace roxlu {

  struct Vec4 {
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
	
    inline Vec4(const Vec3& v3) {
      x = v3.x;
      y = v3.y;
      z = v3.z;
      w = 0.0;
    }
 
    inline Vec4(float v)
      :x(v)
      ,y(v)
      ,z(v)
      ,w(v)
    {
    }

    inline void set(float xx, float yy, float zz, float ww);
    inline float* getPtr() { return &x; }
    inline void print();
    friend std::ostream& operator<<(std::ostream& os, const Vec4& v);
    Vec4& operator =(const Vec3& v3);
	
			
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

  inline void Vec4::print() {
    printf("%f, %f, %f, %f\n", x, y, z, w);
  }

  inline std::ostream& operator<<(std::ostream& os, const Vec4& v) {
    int w = 6;
    os 	<< std::setw(w) << v.x <<", " 
        << setw(w) << v.y << ", " 
        << setw(w) << v.z << ", " 
        << setw(w) << v.w;
    return os;
  }

} // roxlu
#endif
