#ifndef ROXLU_TWITTERCURLVALUETYPESTRINGH
#define ROXLU_TWITTERCURLVALUETYPESTRINGH

#include "TwitterCurlValueType.h"
#include "../../../libs/curl/curl.h"
#include <string>
#include <sstream>

using std::string;

namespace roxlu {

class TwitterCurlValueTypeString : public TwitterCurlValueType {
public:

	template<typename T>
	TwitterCurlValueTypeString(const string& name, const T& val) 
		:name(name)
	{
		std::stringstream ss;
		ss << val;
		value = ss.str();
	}
	
	~TwitterCurlValueTypeString();
	
	virtual void addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last);
	virtual void addToGet(string& data);
	
	string value;
	string name;
};

};

#endif