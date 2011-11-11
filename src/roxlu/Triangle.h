#ifndef ROXLU_TRIANGLEH
#define ROXLU_TRIANGLEH

struct Triangle {
	int a,b,c;
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

#endif