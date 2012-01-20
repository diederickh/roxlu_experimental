#include "TwitterCurlValueTypeString.h"

namespace roxlu {


TwitterCurlValueTypeString::~TwitterCurlValueTypeString() {
	printf("~ string.\n");
}

void TwitterCurlValueTypeString::addToPost(CURL* curl, curl_httppost** curr, curl_httppost** last) {
	curl_formadd(
	 	 curr
		,last
		,CURLFORM_COPYNAME
		,name.c_str()
		,CURLFORM_COPYCONTENTS
		,value.c_str()
		,CURLFORM_END
	);
	printf("Added to post: %s = %s\n", name.c_str(), value.c_str());
}

void TwitterCurlValueTypeString::addToGet(string& data) {
	data += name +"=" +value;
}

string TwitterCurlValueTypeString::getValue() {
	return value;
}

}; // roxlu