#ifndef ROXLU_OSCGTYPEH
#define ROXLU_OSCGTYPEH

#include "OSCGType.h"
#include <string>

namespace roxlu {

enum OSCGTypes {
 	 OSCG_NONE 		= 0
	,OSCG_FLOAT 	= 1
	,OSCG_BOOL 		= 2
	,OSCG_COLOR		= 3
};



class OSCGType {
public:
	OSCGType(const string& name, int type)
		:type(type)
		,name(name)
	{
	}
	
protected:
	int type;
	std::string name;
	
};

} // roxlu
#endif