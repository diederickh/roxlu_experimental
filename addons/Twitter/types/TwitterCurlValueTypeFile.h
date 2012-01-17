#ifndef ROXLU_TWITTERCURLVALUETYEPFILEH
#define ROXLU_TWITTERCURLVALUETYEPFILEH

#include "TwitterCurlValueType.h"
#include "../../../libs/curl/curl.h"
#include <string>

using std::string;

namespace roxlu {

class TwitterCurlValueTypeFile : public TwitterCurlValueType {
public:
	TwitterCurlValueTypeFile(const string& name, const string& filePath);
	~TwitterCurlValueTypeFile();
	virtual void addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last);
	virtual void addToGet(string& data);
	string name;
	string filepath;

};

}; // roxlu

#endif