#include <roxlu/math/Vec3.h>
#include <roxlu/math/Mat3.h>
#include <roxlu/math/Mat4.h>
#include <roxlu/core/Utils.h>

// good resource: https://github.com/Kazade/kazmath/blob/master/kazmath/mat4.c

namespace roxlu {

  // copy
  Mat4::Mat4(const Mat4& o) {
    m[0]  = o.m[0];
    m[1]  = o.m[1];
    m[2]  = o.m[2];
    m[3]  = o.m[3];
    m[4]  = o.m[4];
    m[5]  = o.m[5];
    m[6]  = o.m[6];
    m[7]  = o.m[7];
    m[8]  = o.m[8];
    m[9]  = o.m[9];
    m[10] = o.m[10];
    m[11] = o.m[11];
    m[12] = o.m[12];
    m[13] = o.m[13];
    m[14] = o.m[14];
    m[15] = o.m[15];
  }

  // assignment
  Mat4& Mat4::operator=(const Mat4& o) {
    if(this == &o) {
      return *this;
    }	
	
    m[0]  = o.m[0];
    m[1]  = o.m[1];
    m[2]  = o.m[2];
    m[3]  = o.m[3];
    m[4]  = o.m[4];
    m[5]  = o.m[5];
    m[6]  = o.m[6];
    m[7]  = o.m[7];
    m[8]  = o.m[8];
    m[9]  = o.m[9];
    m[10] = o.m[10];
    m[11] = o.m[11];
    m[12] = o.m[12];
    m[13] = o.m[13];
    m[14] = o.m[14];
    m[15] = o.m[15];

    return *this;
  }

  Mat4::Mat4(const Mat3& o) {
    m[0] = o.m[0];
    m[1] = o.m[1];
    m[2] = o.m[2];
    m[3] = 0.0f;
	
    m[4] = o.m[3];
    m[5] = o.m[4];
    m[6] = o.m[5];
    m[7] = 0.0f;
	
    m[8] = o.m[6];
    m[9] = o.m[7];
    m[10] = o.m[8];
    m[11] = 0.0f;
	
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
  }


  // compare with other mat4
  bool Mat4::operator==(const Mat4& o) const {
    for(int i = 0; i < 16; ++i) {
      if(m[i] != o.m[i]) {
        return false;
      }
    }
    return true;
  }

  bool Mat4::operator!=(const Mat4& o) const {
    for(int i = 0; i < 16; ++i) {
      if(m[i] != o.m[i]) {
        return true;
      }
    }
    return true;
  }


  bool Mat4::isZero() {
    for(int i = 0; i < 16; ++i) {
      if(m[i] != 0.0) {
        return false;
      }
    }
    return true;
  }

  bool Mat4::isIdentity() {	
    if(m[0]  != 1.0f) 	{ 	return false; 	} 
    else if(m[5]  != 1.0f)	{ 	return false;	}
    else if(m[10] != 1.0f)	{ 	return false;	}
    else if(m[15] != 1.0f)	{ 	return false; 	}
    else if(m[1]  != 0.0f)	{ 	return false; 	}
    else if(m[2]  != 0.0f)	{ 	return false; 	}
    else if(m[3]  != 0.0f)	{ 	return false; 	}
    else if(m[4]  != 0.0f)	{ 	return false; 	}
    else if(m[6]  != 0.0f)	{ 	return false; 	}
    else if(m[7]  != 0.0f)	{ 	return false; 	}
    else if(m[8]  != 0.0f)	{ 	return false; 	}
    else if(m[9]  != 0.0f)	{ 	return false; 	}
    else if(m[11] != 0.0f)	{ 	return false; 	}
    else if(m[12] != 0.0f)	{ 	return false; 	}
    else if(m[13] != 0.0f)	{ 	return false; 	}
    else if(m[14] != 0.0f)	{ 	return false; 	}
    else {
      return true;
    }
  }

  void Mat4::identity() {
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

  void Mat4::clean() {
    m[0] = IS_ZERO(m[0]) ? 0.0f : m[0];
    m[1] = IS_ZERO(m[1]) ? 0.0f : m[1];
    m[2] = IS_ZERO(m[2]) ? 0.0f : m[2];
    m[3] = IS_ZERO(m[3]) ? 0.0f : m[3];
    m[4] = IS_ZERO(m[4]) ? 0.0f : m[4];
    m[5] = IS_ZERO(m[5]) ? 0.0f : m[5];
    m[6] = IS_ZERO(m[6]) ? 0.0f : m[6];
    m[7] = IS_ZERO(m[7]) ? 0.0f : m[7];
    m[8] = IS_ZERO(m[8]) ? 0.0f : m[8];
    m[9] = IS_ZERO(m[9]) ? 0.0f : m[9];
    m[10] = IS_ZERO(m[10]) ? 0.0f : m[10];
    m[11] = IS_ZERO(m[11]) ? 0.0f : m[11];
    m[12] = IS_ZERO(m[12]) ? 0.0f : m[12];
    m[13] = IS_ZERO(m[13]) ? 0.0f : m[13];
    m[14] = IS_ZERO(m[14]) ? 0.0f : m[14];
    m[15] = IS_ZERO(m[15]) ? 0.0f : m[15];
  }


  void Mat4::inverse() {
    mat4_inverse(*this, *this);
  }

  Mat4 Mat4::getInverse() {
    Mat4 m = *this;
    m.inverse();
    return m;
  }

#define SWAP_ROWS_DOUBLE(a, b) { double *_tmp = a; (a)=(b); (b)=_tmp; }
#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]
  // thanks: http://www.opengl.org/wiki/GluProject_and_gluUnProject_code
  int mat4_inverse(const Mat4& o, Mat4& result) {
    float wtmp[4][8];
    float m0, m1, m2, m3, s;
    float *r0, *r1, *r2, *r3;
    r0 = wtmp[0];
    r1 = wtmp[1];
    r2 = wtmp[2];
    r3 = wtmp[3];

    r0[0] = MAT(o.m, 0, 0);
    r0[1] = MAT(o.m, 0, 1),
      r0[2] = MAT(o.m, 0, 2);
    r0[3] = MAT(o.m, 0, 3);
	
    r0[4] = 1.0;
    r0[5] = 0.0;
    r0[6] = 0.0;
    r0[7] = 0.0;
	
    r1[0] = MAT(o.m, 1, 0);
    r1[1] = MAT(o.m, 1, 1);
    r1[2] = MAT(o.m, 1, 2);
    r1[3] = MAT(o.m, 1, 3);
	
    r1[5] = 1.0;
    r1[4] = 0.0f;
    r1[6] = 0.0f;
    r1[7] = 0.0f;
	
    r2[0] = MAT(o.m, 2, 0);
    r2[1] = MAT(o.m, 2, 1);
    r2[2] = MAT(o.m, 2, 2);
    r2[3] = MAT(o.m, 2, 3);
	
    r2[6] = 1.0;
    r2[4] = 0.0;
    r2[5] = 0.0;
    r2[7] = 0.0;
	
    r3[0] = MAT(o.m, 3, 0);
    r3[1] = MAT(o.m, 3, 1),
      r3[2] = MAT(o.m, 3, 2);
    r3[3] = MAT(o.m, 3, 3);
	
    r3[7] = 1.0;
    r3[4] = 0.0;
    r3[5] = 0.0;
    r3[6] = 0.0;
 
    // choose pivot - or die 
    if (fabsf(r3[0]) > fabsf(r2[0])) {
      SWAP_ROWS_FLOAT(r3, r2);
    }
	
    if (fabsf(r2[0]) > fabsf(r1[0])) {
      SWAP_ROWS_FLOAT(r2, r1);
    }
	
    if (fabsf(r1[0]) > fabsf(r0[0])) {
      SWAP_ROWS_FLOAT(r1, r0);
    }
    if (0.0 == r0[0]) {
      return 0;
    }
	
    // eliminate first variable     
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
	
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
	
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
	
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
	
    s = r0[4];
    if (s != 0.0) {
      r1[4] -= m1 * s;
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
    }
	
    s = r0[5];
    if (s != 0.0) {
      r1[5] -= m1 * s;
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
    }

    s = r0[6];
    if (s != 0.0) {
      r1[6] -= m1 * s;
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
    }
	
    s = r0[7];
    if (s != 0.0) {
      r1[7] -= m1 * s;
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
    }
	
    // choose pivot - or die 
    if (fabsf(r3[1]) > fabsf(r2[1])) {
      SWAP_ROWS_FLOAT(r3, r2);
    }
    if (fabsf(r2[1]) > fabsf(r1[1])) {
      SWAP_ROWS_FLOAT(r2, r1);
    }
    if (0.0 == r1[1]) {
      return 0;
    }
	
    // eliminate second variable 
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
	
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
	
    s = r1[4];
    if (0.0 != s) {
      r2[4] -= m2 * s;
      r3[4] -= m3 * s;
    }
	
    s = r1[5];
    if (0.0 != s) {
      r2[5] -= m2 * s;
      r3[5] -= m3 * s;
    }
	
    s = r1[6];
    if (0.0 != s) {
      r2[6] -= m2 * s;
      r3[6] -= m3 * s;
    }
	
    s = r1[7];
    if (0.0 != s) {
      r2[7] -= m2 * s;
      r3[7] -= m3 * s;
    }
	
    // choose pivot - or die 
    if (fabsf(r3[2]) > fabsf(r2[2])) {
      SWAP_ROWS_FLOAT(r3, r2);
    }
    if (0.0 == r2[2]) {
      return 0;
    }
	
    // eliminate third variable 
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3];
    r3[4] -= m3 * r2[4];
    r3[5] -= m3 * r2[5];
    r3[6] -= m3 * r2[6];
    r3[7] -= m3 * r2[7];
	
    // last check
    if (0.0 == r3[3]) {
      return 0;
    }
	
    // now back substitute row 3 
    s = 1.0 / r3[3];		
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;
    m2 = r2[3];			
	
    // now back substitute row 2 
    s = 1.0 / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2); 
    r2[5] = s * (r2[5] - r3[5] * m2),
      r2[6] = s * (r2[6] - r3[6] * m2); 
    r2[7] = s * (r2[7] - r3[7] * m2);
	
    m1 = r1[3];
    r1[4] -= r3[4] * m1;
    r1[5] -= r3[5] * m1;
    r1[6] -= r3[6] * m1;
    r1[7] -= r3[7] * m1;
	
    m0 = r0[3];
    r0[4] -= r3[4] * m0;
    r0[5] -= r3[5] * m0;
    r0[6] -= r3[6] * m0;
    r0[7] -= r3[7] * m0;
	
    // now back substitute row 1 
    m1 = r1[2];			
    s = 1.0 / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1);
    r1[5] = s * (r1[5] - r2[5] * m1);
    r1[6] = s * (r1[6] - r2[6] * m1);
    r1[7] = s * (r1[7] - r2[7] * m1);
	
    m0 = r0[2];
    r0[4] -= r2[4] * m0;
    r0[5] -= r2[5] * m0;
    r0[6] -= r2[6] * m0;
    r0[7] -= r2[7] * m0;
	
    // now back substitute row 0 
    m0 = r0[1];			
    s = 1.0 / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0);
    r0[5] = s * (r0[5] - r1[5] * m0);
    r0[6] = s * (r0[6] - r1[6] * m0);
    r0[7] = s * (r0[7] - r1[7] * m0);
	
    MAT(result.m, 0, 0) = r0[4];
    MAT(result.m, 0, 1) = r0[5];
    MAT(result.m, 0, 2) = r0[6];
    MAT(result.m, 0, 3) = r0[7];
    MAT(result.m, 1, 0) = r1[4];
    MAT(result.m, 1, 1) = r1[5];
    MAT(result.m, 1, 2) = r1[6];
    MAT(result.m, 1, 3) = r1[7];
    MAT(result.m, 2, 0) = r2[4];
    MAT(result.m, 2, 1) = r2[5];
    MAT(result.m, 2, 2) = r2[6];
    MAT(result.m, 2, 3) = r2[7];
    MAT(result.m, 3, 0) = r3[4];
    MAT(result.m, 3, 1) = r3[5];
    MAT(result.m, 3, 2) = r3[6];
    MAT(result.m, 3, 3) = r3[7];
    return 1;
  }

  // assumes we are a affine matrix.
  void Mat4::affineInverse() {
    *this = affine_inverse(*this);
  }

  // assumes a standard affine matrix.
  Mat4 affine_inverse(const Mat4& o) {
    Mat4 r;
    float cofactor0 = o.m[5] * o.m[10] - o.m[6] * o.m[9];
    float cofactor4 = o.m[2] * o.m[9]  - o.m[1] * o.m[10];
    float cofactor8 = o.m[1] * o.m[6]  - o.m[2] * o.m[5];

    float det = o.m[0] * cofactor0 + o.m[4] * cofactor4 + o.m[8] * cofactor8;
    if (IS_ZERO(det))  {
      printf("affine inverse error!\n");
      return r;
    }
		
    // upper 3x3
    float inv_det = 1.0f/det;
    r.m[0]  = inv_det * cofactor0;
    r.m[1]  = inv_det * cofactor4;
    r.m[2]  = inv_det * cofactor8;

    r.m[4]  = inv_det * (o.m[6] * o.m[8]  - o.m[4] * o.m[10]);
    r.m[5]  = inv_det * (o.m[0] * o.m[10] - o.m[2] * o.m[8] );
    r.m[6]  = inv_det * (o.m[2] * o.m[4]  - o.m[0] * o.m[6] );
   
    r.m[8]  = inv_det * (o.m[4] * o.m[9]  - o.m[5] * o.m[8] );
    r.m[9]  = inv_det * (o.m[1] * o.m[8]  - o.m[0] * o.m[9] );
    r.m[10] = inv_det * (o.m[0] * o.m[5]  - o.m[1] * o.m[4] );
		
    // translation times invertex 3x3 to get inverse
    r.m[12] = -r.m[0] * o.m[12] - r.m[4] * o.m[13] - r.m[8]  * o.m[14];
    r.m[13] = -r.m[1] * o.m[12] - r.m[5] * o.m[13] - r.m[9]  * o.m[14];
    r.m[14] = -r.m[2] * o.m[12] - r.m[6] * o.m[13] - r.m[10] * o.m[14];

    return r;
  }

#define SWAP_MAT4(a,b,tmp) t = m[a]; m[a] = m[b]; m[b] = t;
  void Mat4::transpose() {
    float t;
    SWAP_MAT4(1,  4,  t);
    SWAP_MAT4(2,  8,  t);
    SWAP_MAT4(3,  12, t);
	
    SWAP_MAT4(6,  9,  t);
    SWAP_MAT4(7,  13, t);
    SWAP_MAT4(11, 14, t);
  }

  Mat4 Mat4::getTranspose() {
    Mat4 m = *this;
    m.transpose();	
    return m;
  }

  Mat4 transpose(const Mat4& o) {
    Mat4 r;
    r.m[0] = o.m[0];
    r.m[1] = o.m[4];
    r.m[2] = o.m[8];
    r.m[3] = o.m[12];
	
    r.m[4] = o.m[1];
    r.m[5] = o.m[5];
    r.m[6] = o.m[9];
    r.m[7] = o.m[13];
	
    r.m[8]  = o.m[2];
    r.m[9]  = o.m[6];
    r.m[10] = o.m[10];
    r.m[11] = o.m[14];
	
    r.m[12] = o.m[3];
    r.m[13] = o.m[7];
    r.m[14] = o.m[11];
    r.m[15] = o.m[15];
	
    return r;
  }

  Mat4 Mat4::translation(const Vec3& v) {
    return translation(v.x, v.y, v.z);
  }

  Mat4 Mat4::translation(const float x, const float y, const float z) {
    Mat4 mat;
    mat.m[0]  = 1.0f;
    mat.m[1]  = 0.0f;
    mat.m[2]  = 0.0f;
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = 0.0f;
    mat.m[5]  = 1.0f;
    mat.m[6]  = 0.0f;
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = 0.0f;
    mat.m[9]  = 0.0f;
    mat.m[10] = 1.0f;
    mat.m[11] = 0.0f;
	
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    mat.m[15] = 1.0f;

    return mat;
  }

  // use rotation from 3x3 mat
  Mat4 Mat4::rotation(const Mat3& o) {
    Mat4 mat;
    mat.m[0]  = o.m[0];
    mat.m[1]  = o.m[1];
    mat.m[2]  = o.m[2];
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = o.m[3];
    mat.m[5]  = o.m[4];
    mat.m[6]  = o.m[5];
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = o.m[6];
    mat.m[9]  = o.m[7];
    mat.m[10] = o.m[8];
    mat.m[11] = 0.0f;
	
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
	
    return mat;
  }

  // from euler angles.
  Mat4 Mat4::rotation(float angleX, float angleY, float angleZ) {
    Mat4 mat;
    //	angleX *= DEG_TO_RAD;
    //	angleY *= DEG_TO_RAD;
    //	angleZ *= DEG_TO_RAD;
	
    float cx = cosf(angleX);
    float sx = sinf(angleX);
    float cy = cosf(angleY);
    float sy = sinf(angleY);
    float cz = cosf(angleZ);
    float sz = sinf(angleZ);

    mat.m[0]  =  (cy * cz);
    mat.m[4]  = -(cy * sz);  
    mat.m[8]  =  sy;
    mat.m[12] = 0.0f;

    mat.m[1]  =  (sx * sy * cz) + (cx * sz);
    mat.m[5]  = -(sx * sy * sz) + (cx * cz);
    mat.m[9]  = -(sx * cy); 
    mat.m[13] = 0.0f;

    mat.m[2]  = -(cx * sy * cz) + (sx * sz);
    mat.m[6]  =  (cx * sy * sz) + (sx * cz);
    mat.m[10] =  (cx * cy);
    mat.m[14] = 0.0f;

    mat.m[3]  = 0.0f;
    mat.m[7]  = 0.0f;
    mat.m[11] = 0.0f;
    mat.m[15] = 1.0f;

    return mat;
  }

  Mat4 Mat4::rotation(float a, float x, float y, float z) {
    //printf("Mat4 rotation is broken!\n"); // fixed! we used: a = DEG_TO_RAD, not: a = a * DEG_TO_RAD
    Mat4 mat;
    //	a = a * DEG_TO_RAD;
    float c = cosf(a);
    float s = sinf(a);

    float t = 1.0f - c;

    Vec3 ax(x,y,z);
    ax.normalize();

    float tx = t * ax.x;  
    float ty = t * ax.y; 
    float tz = t * ax.z;
	
    float sx = s * ax.x;  
    float sy = s * ax.y;
    float sz = s * ax.z;
	
    float txy = tx * ax.y;
    float tyz = tx * ax.z; 
    float txz = tx * ax.z;

    mat.m[0]  = tx * ax.x + c;
    mat.m[4]  = txy - sz; 
    mat.m[8]  = txz + sy;
    mat.m[12] = 0.0f;

    mat.m[1]  = txy + sz;
    mat.m[5]  = ty * ax.y + c;
    mat.m[9]  = tyz - sx;
    mat.m[13] = 0.0f;

    mat.m[2]  = txz - sy;
    mat.m[6]  = tyz + sx;
    mat.m[10] = tz * ax.z + c;
    mat.m[14] = 0.0f;

    mat.m[3]  = 0.0f;
    mat.m[7]  = 0.0f;
    mat.m[11] = 0.0f;
    mat.m[15] = 1.0f;

    return mat;
  }

  void Mat4::rotate(float angle, float x, float y, float z) {
    Mat4 rot = Mat4::rotation(angle, x, y, z);
    *this *= rot;
  }


  void Mat4::setXRotation(const float radians)  {
    float ca = cos(radians);
    float sa = sin(radians);
    m[5] = ca;
    m[6] = -sa;
    m[9] = sa;
    m[10] = ca;
  }

  void Mat4::setYRotation(const float radians)  {
    float ca = cos(radians);
    float sa = sin(radians);
    m[0] = ca;
    m[2] = sa;
    m[8] = -sa;
    m[10] = ca;
  }

  void Mat4::setZRotation(const float radians)  {
    float ca = cos(radians);
    float sa = sin(radians);
    m[0] = ca;
    m[1] = sa;
    m[4] = -sa;
    m[5] = ca;
  }


  Mat4 Mat4::rotationX(float a) {
    Mat4 mat;
    //	a = a * DEG_TO_RAD;
    float c = cosf(a);
    float s = sinf(a);
	
    mat.m[0]  = 1.0f;
    mat.m[1]  = 0.0f;
    mat.m[2]  = 0.0f;
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = 0.0f;
    mat.m[5]  = c;
    mat.m[6]  = s;
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = 0.0f;
    mat.m[9]  = -s;
    mat.m[10] = c;
    mat.m[11] = 0.0f;
	
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
	
    return mat;
  }

  Mat4 Mat4::rotationY(float a) {
    Mat4 mat;
    //a = DEG_TO_RAD;
    //a = a * DEG_TO_RAD;
    float c = cosf(a);
    float s = sinf(a);
	
    mat.m[0]  = c;
    mat.m[1]  = 0.0f;
    mat.m[2]  = -s;
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = 0.0f;
    mat.m[5]  = 1.0f;
    mat.m[6]  = 0.0f;
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = s;
    mat.m[9]  = 0.0f;
    mat.m[10] = c;
    mat.m[11] = 0.0f;
	
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
	
    return mat;
  }


  Mat4 Mat4::rotationZ(float a) {
    Mat4 mat;
    //	a = a * DEG_TO_RAD;
    float c = cosf(a);
    float s = sinf(a);
	
    mat.m[0]  = c;
    mat.m[1]  = s;
    mat.m[2]  = 0.0f;
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = -s;
    mat.m[5]  = c;
    mat.m[6]  = 0.0f;
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = 0.0f;
    mat.m[9]  = 0.0f;
    mat.m[10] = 1.0;
    mat.m[11] = 0.0f;
	
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
	
    return mat;
  }

  Mat4 Mat4::scaling(float x, float y, float z) {
    Mat4 mat;
	
    mat.m[0]  = x;
    mat.m[1]  = 0.0f;
    mat.m[2]  = 0.0f;
    mat.m[3]  = 0.0f;
	
    mat.m[4]  = 0.0f;
    mat.m[5]  = y;
    mat.m[6]  = 0.0f;
    mat.m[7]  = 0.0f;
	
    mat.m[8]  = 0.0f;
    mat.m[9]  = 0.0f;
    mat.m[10] = z;
    mat.m[11] = 0.0f;
	
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
	
    return mat;
  }

  void Mat4::setScale(const float x, const float y, const float z) {
    m[0] = x;
    m[5] = y;
    m[10] = z;
  }

  void Mat4::setScale(const float s) {
    m[0] = s;
    m[5] = s;
    m[10] = s;
  }

  void Mat4::scale(float s) {
    scale(s,s,s);
  }

  void Mat4::scale(float x, float y, float z) {
    Mat4 mat = Mat4::scaling(x,y,z);
    *this *= mat;
  }

  void Mat4::translate(const Vec3& v) {
    Mat4 mat = translation(v.x,v.y,v.z);
    //	printf("=================\n");
    //	print();
    *this *= mat;
    //	mat.print();
    //	printf("----------------\n\n");
	
    /*
      m[12] += v.x;
      m[13] += v.y;
      m[14] += v.z;
    */
	

  }

  void Mat4::translate(float x, float y, float z) {
    Mat4 mat = translation(x,y,z);
    *this *= mat;
  }

  Mat4& Mat4::frustum(float l, float r, float b, float t, float n, float f) {
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;
    m[4]  = 0.0f;
    m[6]  = 0.0f;
    m[7]  = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
	
    m[0]  = 2.0f * n / (r-l);
    m[5]  = 2.0f * n / (t-b);
    m[8]  = (r+l) / (r-l);
    m[9]  = (t+b) / (t-b);
    m[10] = -(f+n) / (f-n);
    m[11] = -1.0f;
    m[14] = -2.0f * f * n / (f-n);
    m[15] = 0.0f;
	
    return *this;
  }

  Mat4& Mat4::perspective(float fovDegrees, float aspect, float n, float f) {
    float hh = tanf(fovDegrees * DEG_TO_RAD * 0.5) * n;
    return frustum(-hh*aspect, hh*aspect, -hh, hh, n, f);
  }

  Mat4& Mat4::frustumCenter(float w, float h, float n, float f) {
    float hw = w * 0.5;
    float hh = h * 0.5;
    return frustum(-hw, hw, -hh, hh, n, f);
  }

  Mat4& Mat4::ortho(float l, float r, float b, float t, float n, float f) {
    m[1]  = 0.0f;
    m[2]  = 0.0f;
    m[3]  = 0.0f;
    m[4]  = 0.0f;
    m[6]  = 0.0f;
    m[7]  = 0.0f;
    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[11] = 0.0f;
    m[15] = 1.0f;

    float rml = r - l;
    float fmn = f - n;
    float tmb = t - b;
    m[0]  = 2.0f / rml;
    m[5]  = 2.0f / tmb;
    m[10] = -2.0f / fmn;
    m[12] = -(r+l)/rml;
    m[13] = -(t+b)/tmb;
    m[14] = -(f+n)/fmn;

    return *this;	
  }

  Mat4& Mat4::orthoCenter(float w, float h, float n, float f) {
    float hw = 0.5 * w;
    float hh = 0.5 * h;
    return ortho(-hw, hw, -hh, -h, n, f);
  }

  Mat4& Mat4::orthoTopLeft(float w, float h, float n, float f) {
    return ortho(0.0f, w, h, 0.0f, n, f);
  }

  Mat4& Mat4::orthoBottomLeft(float w, float h, float n, float f) {
    return ortho(0.0f, w, 0.0f, h, n, f);
  }

  // operators
  //------------------------------------------------------------------------------
  Mat4& Mat4::operator+=(const Mat4& o) {
    m[0]  = m[0]  + o.m[0];
    m[1]  = m[1]  + o.m[1];
    m[2]  = m[2]  + o.m[2];
    m[3]  = m[3]  + o.m[3];
    m[4]  = m[4]  + o.m[4];
    m[5]  = m[5]  + o.m[5];
    m[6]  = m[6]  + o.m[6];
    m[7]  = m[7]  + o.m[7];
    m[8]  = m[8]  + o.m[8];
    m[9]  = m[9]  + o.m[9];
    m[10] = m[10] + o.m[10];
    m[11] = m[11] + o.m[11];
    m[12] = m[12] + o.m[12];
    m[13] = m[13] + o.m[13];
    m[14] = m[14] + o.m[14];
    m[15] = m[15] + o.m[15];
    return *this;
  }

  Mat4 Mat4::operator+(const Mat4& o) const {
    Mat4 r;
    r.m[0]  = m[0]  + o.m[0];
    r.m[1]  = m[1]  + o.m[1];
    r.m[2]  = m[2]  + o.m[2];
    r.m[3]  = m[3]  + o.m[3];
    r.m[4]  = m[4]  + o.m[4];
    r.m[5]  = m[5]  + o.m[5];
    r.m[6]  = m[6]  + o.m[6];
    r.m[7]  = m[7]  + o.m[7];
    r.m[8]  = m[8]  + o.m[8];
    r.m[9]  = m[9]  + o.m[9];
    r.m[10] = m[10] + o.m[10];
    r.m[11] = m[11] + o.m[11];
    r.m[12] = m[12] + o.m[12];
    r.m[13] = m[13] + o.m[13];
    r.m[14] = m[14] + o.m[14];
    r.m[15] = m[15] + o.m[15];
    return r;
  }

  Mat4& Mat4::operator-=(const Mat4& o) {
    m[0]  = m[0]  - o.m[0];
    m[1]  = m[1]  - o.m[1];
    m[2]  = m[2]  - o.m[2];
    m[3]  = m[3]  - o.m[3];
    m[4]  = m[4]  - o.m[4];
    m[5]  = m[5]  - o.m[5];
    m[6]  = m[6]  - o.m[6];
    m[7]  = m[7]  - o.m[7];
    m[8]  = m[8]  - o.m[8];
    m[9]  = m[9]  - o.m[9];
    m[10] = m[10] - o.m[10];
    m[11] = m[11] - o.m[11];
    m[12] = m[12] - o.m[12];
    m[13] = m[13] - o.m[13];
    m[14] = m[14] - o.m[14];
    m[15] = m[15] - o.m[15];
    return *this;
  }

  Mat4 Mat4::operator-(const Mat4& o) const {
    Mat4 r;
    r.m[0]  = m[0]  - o.m[0];
    r.m[1]  = m[1]  - o.m[1];
    r.m[2]  = m[2]  - o.m[2];
    r.m[3]  = m[3]  - o.m[3];
    r.m[4]  = m[4]  - o.m[4];
    r.m[5]  = m[5]  - o.m[5];
    r.m[6]  = m[6]  - o.m[6];
    r.m[7]  = m[7]  - o.m[7];
    r.m[8]  = m[8]  - o.m[8];
    r.m[9]  = m[9]  - o.m[9];
    r.m[10] = m[10] - o.m[10];
    r.m[11] = m[11] - o.m[11];
    r.m[12] = m[12] - o.m[12];
    r.m[13] = m[13] - o.m[13];
    r.m[14] = m[14] - o.m[14];
    r.m[15] = m[15] - o.m[15];

    return r;
  }

  Mat4 Mat4::operator-() const {
    Mat4 r;
    r.m[0]  = -m[0];
    r.m[1]  = -m[1];
    r.m[2]  = -m[2];
    r.m[3]  = -m[3];
    r.m[4]  = -m[4];
    r.m[5]  = -m[5];
    r.m[6]  = -m[6];
    r.m[7]  = -m[7];
    r.m[8]  = -m[8];
    r.m[9]  = -m[9];
    r.m[10] = -m[10];
    r.m[11] = -m[11];
    r.m[12] = -m[12];
    r.m[13] = -m[13];
    r.m[14] = -m[14];
    r.m[15] = -m[15];
    return r;
  }

  Mat4 Mat4::operator*(const Mat4& o) const {
    Mat4 r;
    r.m[0]  =  m[0] * o.m[0]  +  m[4] * o.m[1]  +  m[8]  * o.m[2]  +  m[12] * o.m[3];
    r.m[1]  =  m[1] * o.m[0]  +  m[5] * o.m[1]  +  m[9]  * o.m[2]  +  m[13] * o.m[3];
    r.m[2]  =  m[2] * o.m[0]  +  m[6] * o.m[1]  +  m[10] * o.m[2]  +  m[14] * o.m[3];
    r.m[3]  =  m[3] * o.m[0]  +  m[7] * o.m[1]  +  m[11] * o.m[2]  +  m[15] * o.m[3];
	
    r.m[4]  =  m[0] * o.m[4]  +  m[4] * o.m[5]  +  m[8]  * o.m[6]  +  m[12] * o.m[7];
    r.m[5]  =  m[1] * o.m[4]  +  m[5] * o.m[5]  +  m[9]  * o.m[6]  +  m[13] * o.m[7];
    r.m[6]  =  m[2] * o.m[4]  +  m[6] * o.m[5]  +  m[10] * o.m[6]  +  m[14] * o.m[7];
    r.m[7]  =  m[3] * o.m[4]  +  m[7] * o.m[5]  +  m[11] * o.m[6]  +  m[15] * o.m[7];
	
    r.m[8]  =  m[0] * o.m[8]  +  m[4] * o.m[9]  +  m[8]  * o.m[10] +  m[12] * o.m[11];
    r.m[9]  =  m[1] * o.m[8]  +  m[5] * o.m[9]  +  m[9]  * o.m[10] +  m[13] * o.m[11];
    r.m[10] =  m[2] * o.m[8]  +  m[6] * o.m[9]  +  m[10] * o.m[10] +  m[14] * o.m[11];
    r.m[11] =  m[3] * o.m[8]  +  m[7] * o.m[9]  +  m[11] * o.m[10] +  m[15] * o.m[11];
	
    r.m[12] =  m[0] * o.m[12] +  m[4] * o.m[13] +  m[8]  * o.m[14] +  m[12] * o.m[15];
    r.m[13] =  m[1] * o.m[12] +  m[5] * o.m[13] +  m[9]  * o.m[14] +  m[13] * o.m[15];
    r.m[14] =  m[2] * o.m[12] +  m[6] * o.m[13] +  m[10] * o.m[14] +  m[14] * o.m[15];
    r.m[15] =  m[3] * o.m[12] +  m[7] * o.m[13] +  m[11] * o.m[14] +  m[15] * o.m[15];
    return r;
  }

  Mat4& Mat4::operator*=(const Mat4& o) {

    Mat4 r;
    r.m[0]  =  m[0] * o.m[0]  +  m[4] * o.m[1]  +  m[8]  * o.m[2]  +  m[12] * o.m[3];
    r.m[1]  =  m[1] * o.m[0]  +  m[5] * o.m[1]  +  m[9]  * o.m[2]  +  m[13] * o.m[3];
    r.m[2]  =  m[2] * o.m[0]  +  m[6] * o.m[1]  +  m[10] * o.m[2]  +  m[14] * o.m[3];
    r.m[3]  =  m[3] * o.m[0]  +  m[7] * o.m[1]  +  m[11] * o.m[2]  +  m[15] * o.m[3];
		
    r.m[4]  =  m[0] * o.m[4]  +  m[4] * o.m[5]  +  m[8]  * o.m[6]  +  m[12] * o.m[7];
    r.m[5]  =  m[1] * o.m[4]  +  m[5] * o.m[5]  +  m[9]  * o.m[6]  +  m[13] * o.m[7];
    r.m[6]  =  m[2] * o.m[4]  +  m[6] * o.m[5]  +  m[10] * o.m[6]  +  m[14] * o.m[7];
    r.m[7]  =  m[3] * o.m[4]  +  m[7] * o.m[5]  +  m[11] * o.m[6]  +  m[15] * o.m[7];
	
    r.m[8]  =  m[0] * o.m[8]  +  m[4] * o.m[9]  +  m[8]  * o.m[10] +  m[12] * o.m[11];
    r.m[9]  =  m[1] * o.m[8]  +  m[5] * o.m[9]  +  m[9]  * o.m[10] +  m[13] * o.m[11];
    r.m[10] =  m[2] * o.m[8]  +  m[6] * o.m[9]  +  m[10] * o.m[10] +  m[14] * o.m[11];
    r.m[11] =  m[3] * o.m[8]  +  m[7] * o.m[9]  +  m[11] * o.m[10] +  m[15] * o.m[11];

    r.m[12] =  m[0] * o.m[12] +  m[4] * o.m[13] +  m[8]  * o.m[14] +  m[12] * o.m[15];
    r.m[13] =  m[1] * o.m[12] +  m[5] * o.m[13] +  m[9]  * o.m[14] +  m[13] * o.m[15];
    r.m[14] =  m[2] * o.m[12] +  m[6] * o.m[13] +  m[10] * o.m[14] +  m[14] * o.m[15];
    r.m[15] =  m[3] * o.m[12] +  m[7] * o.m[13] +  m[11] * o.m[14] +  m[15] * o.m[15];
	
    // and copy
    m[0]  = r.m[0];
    m[1]  = r.m[1];
    m[2]  = r.m[2];
    m[3]  = r.m[3];
    m[4]  = r.m[4];
    m[5]  = r.m[5];
    m[6]  = r.m[6];
    m[7]  = r.m[7];
    m[8]  = r.m[8];
    m[9]  = r.m[9];
    m[10] = r.m[10];
    m[11] = r.m[11];
    m[12] = r.m[12];
    m[13] = r.m[13];
    m[14] = r.m[14];
    m[15] = r.m[15];
	
    return *this;
  }

  // mat * vec
  Vec4 Mat4::operator*(const Vec4& v) const {
    Vec4 r;
    r.x = m[0] * v.x  + m[4] * v.y  + m[8]  * v.z + m[12] * v.w;
    r.y = m[1] * v.x  + m[5] * v.y  + m[9]  * v.z + m[13] * v.w;
    r.z = m[2] * v.x  + m[6] * v.y  + m[10] * v.z + m[14] * v.w;
    r.w = m[3] * v.x  + m[7] * v.y  + m[11] * v.z + m[15] * v.w;
    return r;
  }

  // scalar multiplication
  Mat4& Mat4::operator*=(float s) {
    m[0]  *= s;
    m[1]  *= s;
    m[2]  *= s;
    m[3]  *= s;
    m[4]  *= s;
    m[5]  *= s;
    m[6]  *= s;
    m[7]  *= s;
    m[8]  *= s;
    m[9]  *= s;
    m[10] *= s;
    m[11] *= s;
    m[12] *= s;
    m[13] *= s;
    m[14] *= s;
    m[15] *= s;
    return *this;
  }


  // Mat4 m; 
  // float s = 4; 
  // m = m * s;
  Mat4 Mat4::operator*(float s) {
    Mat4 r;
    r.m[0]  = m[0]  * s;
    r.m[1]  = m[1]  * s;
    r.m[2]  = m[2]  * s;
    r.m[3]  = m[3]  * s;
    r.m[4]  = m[4]  * s;
    r.m[5]  = m[5]  * s;
    r.m[6]  = m[6]  * s;
    r.m[7]  = m[7]  * s;
    r.m[8]  = m[8]  * s;
    r.m[9]  = m[9]  * s;
    r.m[10] = m[10] * s;
    r.m[11] = m[11] * s;
    r.m[12] = m[12] * s;
    r.m[13] = m[13] * s;
    r.m[14] = m[14] * s;
    r.m[15] = m[15] * s;
    return r;
  }

  // Mat4 m;
  // float s = 4;
  // m = s * m;
  Mat4 operator*(float s, const Mat4& o) {
    Mat4 r;
    r.m[0]  = o.m[0]  * s;
    r.m[1]  = o.m[1]  * s;
    r.m[2]  = o.m[2]  * s;
    r.m[3]  = o.m[3]  * s;
    r.m[4]  = o.m[4]  * s;
    r.m[5]  = o.m[5]  * s;
    r.m[6]  = o.m[6]  * s;
    r.m[7]  = o.m[7]  * s;
    r.m[8]  = o.m[8]  * s;
    r.m[9]  = o.m[9]  * s;
    r.m[10] = o.m[10] * s;
    r.m[11] = o.m[11] * s;
    r.m[12] = o.m[12] * s;
    r.m[13] = o.m[13] * s;
    r.m[14] = o.m[14] * s;
    r.m[15] = o.m[15] * s;
    return r;
  }

  // get the top 3x3 matrix (rotation)
  Mat3 Mat4::operator=(const Mat4& o) const {
    Mat3 m3(
            o.m[0], o.m[1], o.m[2]
            ,o.m[4], o.m[5], o.m[6]
            ,o.m[8], o.m[9], o.m[10]
            );
    return m3;
  }

  Mat4& Mat4::operator=(const Mat3& o) {	
    m[0] = o.m[0];
    m[1] = o.m[1];
    m[2] = o.m[2];
	
    m[4] = o.m[3];
    m[5] = o.m[4];
    m[6] = o.m[5];
	
    m[8] = o.m[6];
    m[9] = o.m[7];
    m[10] = o.m[8];
	
    return *this;
  }



} // roxlu
