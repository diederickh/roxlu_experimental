#ifndef ROXLU_OSCGUIH
#define ROXLU_OSCGUIH

#include <string>
#include <map>
#include <fstream>

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
	 ,OSCU_COMMAND 	= 7
};

enum OSCUCommands {
	OSCU_COMMAND_LOAD_SETTINGS = 1 // client asks for settings.
};

class OSCGui {
public:
	OSCGui(int receiverPort = 4000, int senderPort = 4001);
	~OSCGui();
	void update();
	void addInt(const string& name, int& v);
	void addFloat(const string& name, float& f);
	void addBool(const string& name, bool& b);
	void addColor(const string& name, float& r, float& g, float& b, float& a);
	void addColor(const string& name, float* rgba);
	void addString(const string& name, string& value);
	void addCallback(const string& name, int value, GuiCallback* type);
	
	bool didValueChange();
	bool save(const string& filepath);
	bool load(const string& filepath);
	void sendValues();
	
	std::string& trim(std::string &s);
 	std::string& ltrim(std::string &s);
	std::string& rtrim(std::string &s);
	
private:
	bool value_changed;
	OSCReceiver osc_receiver;
	OSCSender osc_sender;
	map<string, OSCGType*> elements;
};

inline std::string& OSCGui::trim(std::string &s) {
	return ltrim(rtrim(s));
}

inline std::string& OSCGui::ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

inline std::string& OSCGui::rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

inline bool OSCGui::didValueChange() {
	return value_changed;
}


} // roxlu

#endif