#ifndef ROXLU_OSCGSTRINGH
#define ROXLU_OSCGSTRINGH

#include "OSCGType.h"
#include <string>
using std::string;

namespace roxlu {

class OSCGString : public OSCGType {
public:
	OSCGString(const string& name, string& v)
		:OSCGType(name, OSCG_STRING)	
		,value(v)
	{
	}
	
	void setValue(string v) {
		value = v;
	}
	
private:
	string& value;
};

}
#endif