//#include "StringUtil.h"

#include <roxlu/core/StringUtil.h>

namespace roxlu {
// -----------------------------------------------------------------------------

StringParts::StringParts() {
}

StringParts::~StringParts() {
}

string StringParts::join(string sep) {
	vector<string>::iterator it = parts.begin();
	string result = "";
	while(it != parts.end()) {
		result += (*it);
		if((it+1) != parts.end()) {
			result += sep;
		}
		++it;
	}
	return result;
}

StringParts& StringParts::pop() {
	parts.pop_back();
	return *this;
}

// -----------------------------------------------------------------------------
StringUtil::StringUtil() {
}

StringUtil::StringUtil(string data) {
	value = data;
}

StringUtil::~StringUtil() {
}

StringParts StringUtil::split(const char delim) {
	StringParts str_parts;
	std::stringstream ss(value);
    std::string item;
    while(std::getline(ss, item, delim)) {
		str_parts.addPart(item);
    }
    return str_parts;
}
/* See "File"
string StringUtil::stringFromFile(string path) {
	std::string result = "";
	std::string line = "";
	std::ifstream ifs(path.c_str());
	if(!ifs.is_open()) {
		return result;
	}
	while(getline(ifs,line)) {
		result += line +"\n";
	}
	return result;
}
*/

StringUtil& StringUtil::operator=(const string data) {
	value = data;
	return *this;
}

string StringUtil::str() {
	return value;
}

} // roxlu