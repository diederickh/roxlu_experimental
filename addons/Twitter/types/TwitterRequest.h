#ifndef ROXLU_TWITTERCURLREQUESTH
#define ROXLU_TWITTERCURLREQUESTH

#include <vector>
#include <map>
#include <string>

#include "TwitterCurlValues.h"
#include "../TwitteroAuth.h"

using std::vector;
using std::map;
using std::string;


namespace roxlu {

class TwitterCurlValueType;

class TwitterRequest {
public:
	TwitterRequest(TwitteroAuth& oAuth);
	~TwitterRequest();
	
	void setURL(const string& u);
	string& getURL();
	
	
	// Add a string var-value
	template<typename T>
	void addString(const string& name, const T& value, bool isURLVar = false) {
		TwitterCurlValueType* v = values.addString(name, value);
		if(isURLVar) {
			url_vars.push_back(v);
		}
		else {
			body_vars.push_back(v);
		}
	}
	
	string getHeader(bool includePin = false);
	
	TwitterCurlValues& getValues();	
	
private:
	string url;
	TwitteroAuth& auth;
	TwitterCurlValues values;
	vector<TwitterCurlValueType*> url_vars;
	vector<TwitterCurlValueType*> body_vars;
};

inline TwitterCurlValues& TwitterRequest::getValues() {
	return values;
}

inline void TwitterRequest::setURL(const string& u) {
	url = u;
}

inline string& TwitterRequest::getURL() {
	return url;
}

}; // roxlu
#endif
