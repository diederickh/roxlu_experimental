#ifndef ROXLU_TWITTERVALUETYPEH
#define ROXLU_TWITTERVALUETYPEH

#include "../../../libs/curl/curl.h"
#include <string>

namespace roxlu {

class TwitterCurlValueType {
public:
	virtual void addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last) = 0;
	virtual void addToGet(std::string& data) = 0;
};

}; // roxlu
#endif