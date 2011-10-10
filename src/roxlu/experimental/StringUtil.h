#ifndef ROXLU_STRING
#define ROXLU_STRING

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

using std::string;
using std::vector;

namespace roxlu {

class StringParts {
public:
	StringParts();
	~StringParts();
	string join(string sep);
	StringParts& pop();
	inline void addPart(string part);	
private:
	vector<string> parts;
};

inline void StringParts::addPart(string part) {
	parts.push_back(part);
}


class StringUtil {
public:
	StringUtil();
	StringUtil(string data);
	~StringUtil();
	
	inline StringUtil& set(string data);
	StringParts split(const char delim);
	StringUtil& operator=(const string data);
	string str();
	
	static string stringFromFile(string path);
private:
	string value;
};


inline StringUtil& StringUtil::set(string data) {
	value = data;
	return *this;
}



}
#endif