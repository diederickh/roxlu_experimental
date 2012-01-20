#ifndef ROXLU_TWITTERVALUETYPEH
#define ROXLU_TWITTERVALUETYPEH

#include "../../../libs/curl/curl.h"
#include <string>

namespace roxlu {

class TwitterCurlValueType {
public:
	virtual void addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last) = 0;
	virtual void addToGet(std::string& data) = 0;
	virtual std::string getValue() = 0;
		
	virtual void setName(std::string n) {
		name = n;
	}
	
	virtual std::string getName() {
		return name;
	}
	

	
protected:
	std::string name;

};

}; // roxlu
#endif