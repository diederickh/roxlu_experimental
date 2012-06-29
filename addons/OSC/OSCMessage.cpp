#include "OSCMessage.h"
#include <sstream>

namespace roxlu {

OSCMessage::OSCMessage() {
}

OSCMessage::OSCMessage(const OSCMessage& other) {
	copy(other);
}

OSCMessage& OSCMessage::operator=(const OSCMessage& other) {
	return copy(other);
}

OSCMessage& OSCMessage::copy(const OSCMessage& other) {
	clear();
	address = other.address;
	remote_host = other.remote_host;
	remote_port = other.remote_port;
	
	for(int i = 0; i < other.args.size(); ++i) {
		OSCArgType type = other.getType(i);
		switch(type) {
			case OSCARG_TYPE_INT32: {
				args.push_back(new OSCArgInt32(other.getInt32(i)));
				break;
			}
			case OSCARG_TYPE_FLOAT: {
				args.push_back(new OSCArgFloat(other.getFloat(i)));
				break;
			}
			case OSCARG_TYPE_STRING: {
				args.push_back(new OSCArgString(other.getString(i)));
				break;
			}
			default: {
				printf("error: cannot copy unknown type!\n");
				break;
			}
		}
	}
	return *this;
}

OSCMessage::~OSCMessage() {
	clear();
}

void OSCMessage::clear() {
	vector<OSCArg*>::iterator it = args.begin();
	while(it != args.end()) {
		delete *it;
		it = args.erase(it);
	}
}

OSCArgType OSCMessage::getType(int dx) const {
	if(dx >= args.size()) {
		printf("error: out of bound.\n");
		return OSCARG_TYPE_OUTOFBOUND;
	} 
	return args[dx]->getType();
}

string OSCMessage::getString(int dx) const {
	OSCArgType type = getType(dx);
	if(type == OSCARG_TYPE_STRING) {
		return ((OSCArgString*)args[dx])->get();
	}
	else if(type == OSCARG_TYPE_INT32) {
		std::stringstream ss;
		ss << ((OSCArgInt32*)args[dx])->get();
		return ss.str();
	}
	else if(type == OSCARG_TYPE_FLOAT) {
		std::stringstream ss;
		ss << ((OSCArgFloat*)args[dx])->get();
		return ss.str();
	}
	else {
		printf("error: not a string type.\n");
		return "";
	}
}

int32_t OSCMessage::getInt32(int dx) const {
	OSCArgType type = getType(dx);
	if(type == OSCARG_TYPE_INT32) {
		return ((OSCArgInt32*)args[dx])->get();
	}
	else if(type == OSCARG_TYPE_FLOAT) {
		printf("warning: float to int conversion\n");
		return ((OSCArgFloat*)args[dx])->get();
	}
	else {
		printf("error: not a int type.\n");
		return 0;
	}
}

float OSCMessage::getFloat(int dx) const {
	OSCArgType type = getType(dx);
	if(type == OSCARG_TYPE_FLOAT) {
		return ((OSCArgFloat*)args[dx])->get();
	}
	else if(type == OSCARG_TYPE_INT32) {
		printf("warning: into to float conversion\n");
		return ((OSCArgInt32*)args[dx])->get();
	}
	else {
		printf("error: not a float type.\n");
		return 0;
	}
}


void OSCMessage::addString(const string& str) {
	args.push_back(new OSCArgString(str));
}

void OSCMessage::addInt32(const int32_t i) {
	args.push_back(new OSCArgInt32(i));
}

void OSCMessage::addFloat(const float f) {
	args.push_back(new OSCArgFloat(f));
}



} // roxlu