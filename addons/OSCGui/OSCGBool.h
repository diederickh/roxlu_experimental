#ifndef ROXLU_OSCGBOOLH
#define ROXLU_OSCGBOOLH


#include "OSCGType.h"

namespace roxlu {

class OSCGBool : public OSCGType {
public:
	OSCGBool(const string& name, bool& v)
		:OSCGType(name, OSCG_BOOL)	
		,value(v)
	{
	}
	
	void setValue(bool v) {
		value = v;
	}
	
	bool getStringValue(std::string& result) {
		std::stringstream ss;
		ss << value;
		result = ss.str();
		return true;
	}
	
	bool setStringValue(std::string& value)  {
		return true;
	}
	
private:
	bool& value;
};

}

#endif