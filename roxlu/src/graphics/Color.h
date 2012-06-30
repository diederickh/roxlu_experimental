#ifndef ROXLU_COLOR4H
#define ROXLU_COLOR4H

// TODO: implement HSL See Graphics Gems 1, HSLtoRGB.c

#define COLOR_CLAMP(a, minv, maxv) ( ((a) < (minv)) ? minv : ( (a) > (maxv) ? maxv : a ) )
 
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

struct Color {	
	
	static void RGBToHLSf(float rr, float gg, float bb, float* hh, float *ll, float *ss) {
		// Compute HLS from RGB. The r,g,b triplet is between [0,1], 
		// hue is between [0,360], light and saturation are [0,1].

		float rnorm, gnorm, bnorm, minval, maxval, msum, mdiff, r, g, b;
		r = g = b = 0;
		if(rr>0) r = rr; if(r>1) r = 1;
		if(gg>0) g = gg; if(g>1) g = 1;
		if(bb>0) b = bb; if(b>1) b = 1;

		minval = r;
		if(g<minval) minval = g;
		if(b<minval) minval = b;
		maxval = r;
		if(g>maxval) maxval = g;
		if(b>maxval) maxval = b;

		rnorm = gnorm = bnorm = 0;
		mdiff = maxval - minval;
		msum  = maxval + minval;
		float l = 0.5f * msum;
		
		if(ll) {
			*ll = l;
		}
		
		if(maxval!=minval) 	{
			rnorm = (maxval - r)/mdiff;
			gnorm = (maxval - g)/mdiff;
			bnorm = (maxval - b)/mdiff;
		} 
		else {
			if(ss) {
				*ss = 0;
			}
			if(hh) {
				*hh = 0;
			}
			return;
		}

		if(ss) {
			if(l<0.5f) {
				*ss = mdiff/msum;
			}
			else {
				*ss = mdiff/(2.0f - msum);
			}
		}

		if(hh) {
			if(r==maxval) {
				*hh = 60.0f * (6.0f + bnorm - gnorm);
			}
			else if(g==maxval) {
				*hh = 60.0f * (2.0f + rnorm - bnorm);
			}
			else {
				*hh = 60.0f * (4.0f + gnorm - rnorm);
			}

			if(*hh>360.0f) {
				*hh -= 360.0f;
			}
		}
	}
	
	static void RGBToHLSi(int rr, int gg, int bb, int *hh, int *ll, int *ss) {
		float h, l, s;
		
		Color::RGBToHLSf(
			 (1.0f/255.0f)*float(rr)
			,(1.0f/255.0f)*float(gg)
			,(1.0f/255.0f)*float(bb)
			,&h
			,&l
			,&s
		);
		
		if(hh) {
			*hh = (int)COLOR_CLAMP(h*(256.0f/360.0f), 0.0f, 255.0f);
		}
		
		if(ll) {
			*ll = (int)COLOR_CLAMP(l*256.0f, 0.0f, 255.0f);
		}
		
		if(ss) {
			*ss= (int)COLOR_CLAMP(s*256.0f, 0.0f, 255.0f);
		}
	}
	
	static void HLSToRGBf(float hh, float ll, float ss, float *rr, float *gg, float *bb) {
		// Compute RGB from HLS. The light and saturation are between [0,1]
		// and hue is between [0,360]. The returned r,g,b triplet is between [0,1].

		// a local auxiliary function
		struct HSLToRGBf_Local {
			static float HLSToRGB(float rr1, float rr2, float hh) {
				float hue = hh;
				if(hue>360) hue = hue - 360;
				if(hue<0)   hue = hue + 360;
				if(hue<60 ) return rr1 + (rr2-rr1*hue/60);
				if(hue<180) return rr2;
				if(hue<240) return rr1 + (rr2-rr1)*(240-hue)/60;
				return rr1;
			}
		};

		float rh, rl, rs, rm1, rm2;
		rh = rl = rs = 0;
		if(hh > 0) rh = hh;     if(rh>360) rh = 360;
		if(ll > 0) rl = ll;     if(rl>1)   rl = 1;
		if(ss > 0) rs = ss; 	if(rs>1)   rs = 1;

		if(rl<=0.5f) {
			rm2 = rl*(1.0f + rs);
		}
		else {
			rm2 = rl + rs - rl*rs;
		}
		rm1 = 2.0f*rl - rm2;

		if(!rs) { 
			if(rr) *rr = rl; 
			if(gg) *gg = rl; 
			if(bb) *bb = rl; 
		}
		else {
			if(rr) *rr = HSLToRGBf_Local::HLSToRGB(rm1, rm2, rh+120);
			if(gg) *gg = HSLToRGBf_Local::HLSToRGB(rm1, rm2, rh);
			if(bb) *bb = HSLToRGBf_Local::HLSToRGB(rm1, rm2, rh-120);
		}
	}
	
	static void HLSToRGBi(int hh, int ll, int ss, int *rr, int *gg, int *bb) {
	    float r, g, b;
		Color::HLSToRGBf(
			 (360.0f/255.0f)*float(hh)
			,(1.0f/255.0f)*float(ll)
			,(1.0f/255.0f)*float(ss)
			,&r
			,&g
			,&b
		);
		
		if(rr) *rr = (int)COLOR_CLAMP(r*256.0f, 0.0f, 255.0f);
		if(gg) *gg = (int)COLOR_CLAMP(g*256.0f, 0.0f, 255.0f);
		if(bb) *bb = (int)COLOR_CLAMP(b*256.0f, 0.0f, 255.0f);
	}

};

#endif
