#ifndef ROXLU_COLOR4H
#define ROXLU_COLOR4H

// TODO: implement HSL See Graphics Gems 1, HSLtoRGB.c

// color
struct Color4 {
	float r,g,b,a;
	
	Color4():r(1.0f),g(1.0f),b(1.0f),a(1.0f) {}
	
	Color4(float nR, float nG, float nB, float nA)
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
	
	float* getPtr() {
		return &r;
		
	}
};

#endif
