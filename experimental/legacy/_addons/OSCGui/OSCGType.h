#ifndef ROXLU_OSCGTYPEH
#define ROXLU_OSCGTYPEH

#include "OSCGType.h"
#include <string>
#include <sstream>

namespace roxlu {

enum OSCGTypes {
 	 OSCG_NONE 		= 0
	,OSCG_FLOAT 	= 1
	,OSCG_BOOL 		= 2
	,OSCG_COLOR		= 3
	,OSCG_INT		= 4
	,OSCG_STRING	= 5
	,OSCG_CALLBACK	= 6
};



class OSCGType {
public:
	OSCGType(const string& name, int type)
		:type(type)
		,name(name)
	{
	}
	
	virtual bool getStringValue(std::string& result) = 0;
	virtual bool setStringValue(std::string& value) = 0;

protected:
	int type;
	std::string name;
	
};

} // roxlu
#endif