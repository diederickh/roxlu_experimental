#ifndef ROXLU_OSCGCALLBACKH
#define ROXLU_OSCGCALLBACKH

#include "OSCGType.h"


namespace roxlu {

class GuiCallback {
public:
	virtual void guiCallback(int v) = 0;
};

class OSCGCallback : public OSCGType {
public:
	OSCGCallback(const string& name, int value, GuiCallback* ptr)
		:OSCGType(name, OSCG_CALLBACK)	
		,ptr(ptr)
		,value(value)
	{
	}
	
	void call() {
		ptr->guiCallback(value);
	}	
	
	bool getStringValue(std::string& result) {
		return false;
	}
	
	bool setStringValue(std::string& value)  {
		return false;
	}
	
	int value;
	GuiCallback* ptr;
};

}
#endif