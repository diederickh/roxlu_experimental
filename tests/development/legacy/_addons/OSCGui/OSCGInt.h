#ifndef ROXLU_OSCGINTH
#define ROXLU_OSCGINTH

#include "OSCGType.h"

namespace roxlu {

class OSCGInt : public OSCGType {
public:
	OSCGInt(const string& name, int& v)
		:OSCGType(name, OSCG_INT)	
		,value(v)
	{
	}
	
	void setValue(int v) {
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
	int& value;
};

}

#endif