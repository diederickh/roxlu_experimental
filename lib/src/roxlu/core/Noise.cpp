//#include "Noise.h"

#include <roxlu/core/Noise.h>

#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

namespace roxlu {

unsigned char perm[512] = {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
  151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 
};

float  grad1( int hash, float x ) {
    int h = hash & 15;
    float grad = 1.0f + (h & 7); 
    if (h&8) grad = -grad;       
    return ( grad * x );         
}

float  grad2( int hash, float x, float y ) {
    int h = hash & 7;      
    float u = h<4 ? x : y;  
    float v = h<4 ? y : x;  
    return ((h&1)? -u : u) + ((h&2)? -2.0f*v : 2.0f*v);
}

float  grad3( int hash, float x, float y , float z ) {
    int h = hash & 15;    
    float u = h<8 ? x : y; 
    float v = h<4 ? y : h==12||h==14 ? x : z; 
    return ((h&1)? -u : u) + ((h&2)? -v : v);
}

float  grad4( int hash, float x, float y, float z, float t ) {
    int h = hash & 31;     
    float u = h<24 ? x : y; 
    float v = h<16 ? y : z;
    float w = h<8 ? z : t;
    return ((h&1)? -u : u) + ((h&2)? -v : v) + ((h&4)? -w : w);
}

static unsigned char simplex[64][4] = {
    {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
    {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
    {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
    {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}};


float noise1(float x) {

  int i0 = FASTFLOOR(x);
  int i1 = i0 + 1;
  float x0 = x - i0;
  float x1 = x0 - 1.0f;

  float n0, n1;

  float t0 = 1.0f - x0*x0;

  t0 *= t0;
  n0 = t0 * t0 * grad1(perm[i0 & 0xff], x0);

  float t1 = 1.0f - x1*x1;
  t1 *= t1;
  n1 = t1 * t1 * grad1(perm[i1 & 0xff], x1);
  return 0.25f * (n0 + n1);

}

float noise2(float x, float y) {

#define F2 0.366025403 
#define G2 0.211324865 

    float n0, n1, n2;
	 float s = (x+y)*F2; 
    float xs = x + s;
    float ys = y + s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);

    float t = (float)(i+j)*G2;
    float X0 = i-t; 
    float Y0 = j-t;
    float x0 = x-X0; 
    float y0 = y-Y0;

    int i1, j1;
    if(x0>y0) {i1=1; j1=0;}
    else {i1=0; j1=1;}

    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2; 
    float y2 = y0 - 1.0f + 2.0f * G2;


    int ii = i & 0xff;
    int jj = j & 0xff;

    float t0 = 0.5f - x0*x0-y0*y0;
    if(t0 < 0.0f) n0 = 0.0f;
    else {
      t0 *= t0;
      n0 = t0 * t0 * grad2(perm[ii+perm[jj]], x0, y0); 
    }

    float t1 = 0.5f - x1*x1-y1*y1;
    if(t1 < 0.0f) n1 = 0.0f;
    else {
      t1 *= t1;
      n1 = t1 * t1 * grad2(perm[ii+i1+perm[jj+j1]], x1, y1);
    }

    float t2 = 0.5f - x2*x2-y2*y2;
    if(t2 < 0.0f) n2 = 0.0f;
    else {
      t2 *= t2;
      n2 = t2 * t2 * grad2(perm[ii+1+perm[jj+1]], x2, y2);
    }

    return 40.0f * (n0 + n1 + n2);
  }


float noise3(float x, float y, float z) {


#define F3 0.333333333
#define G3 0.166666667

    float n0, n1, n2, n3;

    float s = (x+y+z)*F3; 
	float xs = x+s;
    float ys = y+s;
    float zs = z+s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);
    int k = FASTFLOOR(zs);

    float t = (float)(i+j+k)*G3; 
    float X0 = i-t;
    float Y0 = j-t;
    float Z0 = k-t;
    float x0 = x-X0;
    float y0 = y-Y0;
    float z0 = z-Z0;

    int i1, j1, k1; 
    int i2, j2, k2; 


    if(x0>=y0) {
      if(y0>=z0)
        { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } 
        else if(x0>=z0) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; }
        else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } 
      }
    else { 
      if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } 
      else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } 
      else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } 
    }

    float x1 = x0 - i1 + G3; 
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f*G3; 
    float y2 = y0 - j2 + 2.0f*G3;
    float z2 = z0 - k2 + 2.0f*G3;
    float x3 = x0 - 1.0f + 3.0f*G3; 
    float y3 = y0 - 1.0f + 3.0f*G3;
    float z3 = z0 - 1.0f + 3.0f*G3;

	int ii = i & 0xff;
    int jj = j & 0xff;
    int kk = k & 0xff;

    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if(t0 < 0.0f) n0 = 0.0f;
    else {
      t0 *= t0;
      n0 = t0 * t0 * grad3(perm[ii+perm[jj+perm[kk]]], x0, y0, z0);
    }

    float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if(t1 < 0.0f) n1 = 0.0f;
    else {
      t1 *= t1;
      n1 = t1 * t1 * grad3(perm[ii+i1+perm[jj+j1+perm[kk+k1]]], x1, y1, z1);
    }

    float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if(t2 < 0.0f) n2 = 0.0f;
    else {
      t2 *= t2;
      n2 = t2 * t2 * grad3(perm[ii+i2+perm[jj+j2+perm[kk+k2]]], x2, y2, z2);
    }

    float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if(t3<0.0f) n3 = 0.0f;
    else {
      t3 *= t3;
      n3 = t3 * t3 * grad3(perm[ii+1+perm[jj+1+perm[kk+1]]], x3, y3, z3);
    }

    return 32.0f * (n0 + n1 + n2 + n3); 
}


float noise4(float x, float y, float z, float w) {
  

#define F4 0.309016994 
#define G4 0.138196601 
    float n0, n1, n2, n3, n4;
   
    float s = (x + y + z + w) * F4; 
    float xs = x + s;
    float ys = y + s;
    float zs = z + s;
    float ws = w + s;
    int i = FASTFLOOR(xs);
    int j = FASTFLOOR(ys);
    int k = FASTFLOOR(zs);
    int l = FASTFLOOR(ws);

    float t = (i + j + k + l) * G4; 
    float X0 = i - t; 
    float Y0 = j - t;
    float Z0 = k - t;
    float W0 = l - t;

    float x0 = x - X0; 
    float y0 = y - Y0;
    float z0 = z - Z0;
    float w0 = w - W0;

    int c1 = (x0 > y0) ? 32 : 0;
    int c2 = (x0 > z0) ? 16 : 0;
    int c3 = (y0 > z0) ? 8 : 0;
    int c4 = (x0 > w0) ? 4 : 0;
    int c5 = (y0 > w0) ? 2 : 0;
    int c6 = (z0 > w0) ? 1 : 0;
    int c = c1 + c2 + c3 + c4 + c5 + c6;

    int i1, j1, k1, l1;
    int i2, j2, k2, l2;
    int i3, j3, k3, l3;

    i1 = simplex[c][0]>=3 ? 1 : 0;
    j1 = simplex[c][1]>=3 ? 1 : 0;
    k1 = simplex[c][2]>=3 ? 1 : 0;
    l1 = simplex[c][3]>=3 ? 1 : 0;

    i2 = simplex[c][0]>=2 ? 1 : 0;
    j2 = simplex[c][1]>=2 ? 1 : 0;
    k2 = simplex[c][2]>=2 ? 1 : 0;
    l2 = simplex[c][3]>=2 ? 1 : 0;

    i3 = simplex[c][0]>=1 ? 1 : 0;
    j3 = simplex[c][1]>=1 ? 1 : 0;
    k3 = simplex[c][2]>=1 ? 1 : 0;
    l3 = simplex[c][3]>=1 ? 1 : 0;

    float x1 = x0 - i1 + G4;
    float y1 = y0 - j1 + G4;
    float z1 = z0 - k1 + G4;
    float w1 = w0 - l1 + G4;
    float x2 = x0 - i2 + 2.0f*G4; 
    float y2 = y0 - j2 + 2.0f*G4;
    float z2 = z0 - k2 + 2.0f*G4;
    float w2 = w0 - l2 + 2.0f*G4;
    float x3 = x0 - i3 + 3.0f*G4;
    float y3 = y0 - j3 + 3.0f*G4;
    float z3 = z0 - k3 + 3.0f*G4;
    float w3 = w0 - l3 + 3.0f*G4;
    float x4 = x0 - 1.0f + 4.0f*G4; 
    float y4 = y0 - 1.0f + 4.0f*G4;
    float z4 = z0 - 1.0f + 4.0f*G4;
    float w4 = w0 - 1.0f + 4.0f*G4;


    int ii = i & 0xff;
    int jj = j & 0xff;
    int kk = k & 0xff;
    int ll = l & 0xff;


    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0 - w0*w0;
    if(t0 < 0.0f) n0 = 0.0f;
    else {
      t0 *= t0;
      n0 = t0 * t0 * grad4(perm[ii+perm[jj+perm[kk+perm[ll]]]], x0, y0, z0, w0);
    }

   float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
    if(t1 < 0.0f) n1 = 0.0f;
    else {
      t1 *= t1;
      n1 = t1 * t1 * grad4(perm[ii+i1+perm[jj+j1+perm[kk+k1+perm[ll+l1]]]], x1, y1, z1, w1);
    }

   float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
    if(t2 < 0.0f) n2 = 0.0f;
    else {
      t2 *= t2;
      n2 = t2 * t2 * grad4(perm[ii+i2+perm[jj+j2+perm[kk+k2+perm[ll+l2]]]], x2, y2, z2, w2);
    }

   float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
    if(t3 < 0.0f) n3 = 0.0f;
    else {
      t3 *= t3;
      n3 = t3 * t3 * grad4(perm[ii+i3+perm[jj+j3+perm[kk+k3+perm[ll+l3]]]], x3, y3, z3, w3);
    }

   float t4 = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
    if(t4 < 0.0f) n4 = 0.0f;
    else {
      t4 *= t4;
      n4 = t4 * t4 * grad4(perm[ii+1+perm[jj+1+perm[kk+1+perm[ll+1]]]], x4, y4, z4, w4);
    }


    return 27.0f * (n0 + n1 + n2 + n3 + n4); 
}


float fbm(const Vec2& v) {
	float result = 0.0f;
	float amp = 0.5f;
	float x = v.x;
	float y = v.y;
	for(rx_uint8 i = 0; i < 4; ++i) {
		result += noise2(x,y) * amp;
		x *= 2.0f;
		y *= 2.0f;
		amp *= 0.5f;
	}
	return result;
}



}; // roxlu