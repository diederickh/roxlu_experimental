#ifndef ROXLU_OSCGUIH
#define ROXLU_OSCGUIH

#include <string>
#include <map>

#include "OSC.h"
#include "OSCGType.h"
#include "OSCGFloat.h"
#include "OSCGBool.h"
#include "OSCGColor.h"
#include "OSCGInt.h"
#include "OSCGString.h"
#include "OSCGCallback.h"


using std::map;
using std::string;

namespace roxlu {

// Osculator type
enum OSCUTypes {
	  OSCU_SLIDER 	= 1
	 ,OSCU_BOOL 	= 2
	 ,OSCU_COLOR	= 3
	 ,OSCU_MATRIX	= 4
	 ,OSCU_STRING	= 5
	 ,OSCU_CALLBACK	= 6
};

class OSCGui {
public:
	OSCGui(int port);
	~OSCGui();
	void update();
	void addInt(const string& name, int& v);
	void addFloat(const string& name, float& f);
	void addBool(const string& name, bool& b);
	void addColor(const string& name, float& r, float& g, float& b, float& a);
	void addColor(const string& name, float* rgba);
	void addString(const string& name, string& value);
	void addCallback(const string& name, int value, GuiCallback* type);
private:
	OSCReceiver osc_receiver;
	map<string, OSCGType*> elements;
};

} // roxlu

#endif