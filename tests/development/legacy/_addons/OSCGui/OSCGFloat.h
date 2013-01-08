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
	
	bool getStringValue(std::string& result) {
		std::stringstream ss;
		ss << value;
		result = ss.str();
		return true;
	}
	
	bool setStringValue(std::string& v)  {
		return true;
	}
	
	
private:
	float& value;
};

}
#endif