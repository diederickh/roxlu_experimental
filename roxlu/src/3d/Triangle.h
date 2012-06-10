#ifndef ROXLU_TRIANGLEH
#define ROXLU_TRIANGLEH

#include "Vec3.h"
namespace roxlu {

struct Triangle {
	int a,b,c;
	Vec3 normal;
	
	Triangle(int nA, int nB, int nC):a(nA),b(nB),c(nC)
	{
        
	}
	void set(int nA, int nB, int nC) {
		a = nA;
		b = nB;
		c = nC;
	}
		
	int& operator[](int n){
		return (&a)[n];
	}
};

} // roxlu
#endif