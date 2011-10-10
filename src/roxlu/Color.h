#ifndef ROXLU_COLOR4FH
#define ROXLU_COLOR4FH

// color
struct Color4f {
	float r,g,b,a;
	
	Color4f():r(1.0f),g(1.0f),b(1.0f),a(1.0f) {}
	
	Color4f(float nR, float nG, float nB, float nA)
		:r(nR),g(nG),b(nB),a(nA)
	{
	}
	
	void set(float nR, float nG, float nB, float nA) {
		r = nR;
		g = nG;
		b = nB;
		a = nA;
	}
	void set(float nR, float nG, float nB) {
		set(nR, nG, nB, 1.0f);
	}
};

#endif
