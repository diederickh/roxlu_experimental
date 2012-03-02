#ifndef OSCMESSAGEH
#define OSCMESSAGEH

#include <string>
#include <vector>
#include "OSCArg.h"

using std::vector;
using std::string;

namespace roxlu {

class OSCMessage {
public:
	OSCMessage();
	OSCMessage(const OSCMessage& other);
	OSCMessage& operator=(const OSCMessage& other);
	OSCMessage& copy(const OSCMessage& other);
	
	~OSCMessage();
	void clear();
	
	string getAddress();
	void setAddress(const string& addr);
	void setRemoteEndpoint(const string& host, int port);

	
	void addString(const string& str);
	void addInt32(const int32_t i);
	void addFloat(const float f); 
	
	OSCArgType getType(int dx) const;
	string getString(int dx) const;
	int32_t getInt32(int dx) const;
	float getFloat(int dx) const;
	
	typedef vector<OSCArg*>::iterator iterator;
	vector<OSCArg*>::iterator begin();
	vector<OSCArg*>::iterator end();
	
private:
	string remote_host;
	int remote_port;
	vector<OSCArg*> args;
	string address;
};

inline void OSCMessage::setAddress(const string& addr) {
	address = addr;
}

inline string OSCMessage::getAddress() {
	return address;	
}

inline vector<OSCArg*>::iterator OSCMessage::begin() {
	return args.begin();
}

inline vector<OSCArg*>::iterator OSCMessage::end() {
	return args.end();
}

inline void OSCMessage::setRemoteEndpoint(const string& host, int port) {
	remote_host = host; 
	remote_port = port; 
}


}
#endif