#ifndef TRIANGLEH
#define TRIANGLEH

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
};

#endif