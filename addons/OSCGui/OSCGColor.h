#ifndef ROXLU_OSCGCOLORH
#define ROXLU_OSCGCOLORH

namespace roxlu {

class OSCGColor : public OSCGType {
public:
	OSCGColor(const string& name, float& r ,float& g, float& b, float& a)
		:OSCGType(name, OSCG_COLOR)	
		,r(r)
		,g(g)
		,b(b)
		,a(a)
	{
	}
	
	void setValue(float rr, float gg, float bb, float aa) {
		r = rr;
		g = gg;
		b = bb;
		a = aa;
	}
	
private:
	float& r;
	float& g;
	float& b;
	float& a;

};

}


#endif

