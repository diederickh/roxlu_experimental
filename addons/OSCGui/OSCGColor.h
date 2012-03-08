#ifndef ROXLU_OSCGCOLORH
#define ROXLU_OSCGCOLORH

#include "OSCGType.h"
#include <string>
#include <sstream>

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
	
	bool getStringValue(std::string& result) {
		std::stringstream ss;
		ss << r << "," << g << "," << b << "," << a;
		result = ss.str();
		return true;
	}
	
	bool setStringValue(std::string& value)  {	
		std::stringstream ss(value);
		int c = 0;
		vector<float> vals;
		std::string v;
		while(std::getline(ss,v,',')) {
			std::stringstream vs(v); 
			float col = 0.0f;
			vs >> col;
			vals.push_back(col);
		}
		
		if(vals.size() == 4) {
			a = vals[3];
		}
		if(vals.size() >= 3) {
			r = vals[0];
			g = vals[1];
			b = vals[2];
		}
		return true;
	}
	
	
private:
	float& r;
	float& g;
	float& b;
	float& a;

};

}


#endif

