#ifndef ROXLU_OSCGFLOATH
#define ROXLU_OSCGFLOATH

#include "OSCGType.h"

namespace roxlu {

class OSCGFloat : public OSCGType {
public:
	OSCGFloat(const string& name, float& v)
		:OSCGType(name, OSCG_FLOAT)	
		,value(v)
	{
	}
	
	void setValue(float v) {
		value = v;
	}
	
private:
	float& value;
};

}
#endif