#include "TwitterRequest.h"

namespace roxlu {

TwitterRequest::TwitterRequest(TwitteroAuth& oAuth) 
	:auth(oAuth)
{
}

TwitterRequest::~TwitterRequest() {
}


string TwitterRequest::getHeader(bool includePin) {
	printf("send\n");
	
	// create a key-value pair 
	map<string,string> key_values;
	for(int i = 0; i < url_vars.size(); ++i) {
		key_values[url_vars[i]->getName()] = url_vars[i]->getValue();
	}
/*	
	 TwitteroAuth::getHeader(
			 const RequestType type
			,const string url
			,const map<string, string>* extraData
			,string& header 
			,const bool includePin 

)
*/
	string header;
	auth.getHeader(TwitteroAuth::TWITTER_OAUTH_GET, getURL(), &key_values, header, false);
	return header;

}


}; // roxlu