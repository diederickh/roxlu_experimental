#include "Twitter.h"
namespace roxlu {

Twitter::Twitter() 
	:curl_param_callback_set(false)
	,curl_param_login_set(false)
{
	curl = curl_easy_init();
	if(curl == NULL) {
		printf("Error: cannot initialize curl for twitter.\n");
	}
}

Twitter::~Twitter() {
}

bool Twitter::oAuthRequestToken(string& authURL /* out */) {
	if(!isCurlInitialized()) {
		return false;
	}
	
	string header;
	bool result = false;
	result = auth.getHeader(
						 TwitteroAuth::TWITTER_OAUTH_GET
						,roxlu::twitter::REQUEST_TOKEN_URL
						,""
						,header
	);
	
	if(!result) {
		return false;
	}

	// get the token key and secret	
	if(performGet(roxlu::twitter::REQUEST_TOKEN_URL, header)) {
		if(!auth.extractTokenKeyAndSecret(buffer)) {
			printf("Error while extracting token key and secrect from response");
			return false;
		}
		authURL.assign(roxlu::twitter::AUTHORIZE_URL);
		authURL.append(auth.getTokenKey());
	}
	return true;
}

bool Twitter::performGet(const string& url, const string& oAuthHeader) {
	struct curl_slist* header_list = NULL;
	
	prepareCurlStandardParams();
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
	// append oauth headers to http-headers
	if(oAuthHeader.length()) {
		header_list = curl_slist_append(header_list, oAuthHeader.c_str());
		if(header_list) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		}
	}
	
	CURLcode r = curl_easy_perform(curl);
	
	if(header_list) {
		curl_slist_free_all(header_list);
	}
	return (r == CURLE_OK);
}

// Curl internals
// -----------------------------------------------------------------------------
void Twitter::prepareCurlCallback() {
	if(!curl_param_callback_set) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		curl_param_callback_set = true;
	}
}

void Twitter::prepareCurlUserPass() {
	if(!curl_param_login_set) {
		string userpass = twitter_username +":" +twitter_password;
		curl_easy_setopt(curl, CURLOPT_USERPWD, NULL); // resets current values
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass.c_str());
		curl_param_login_set = true;
	}
}

void Twitter::prepareCurlStandardParams() {
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL); // reset request, from twitcurl.
	prepareCurlCallback();
	prepareCurlUserPass();
}

size_t Twitter::curlCallback(char* data, size_t size, size_t nmemb, Twitter* twit) {
	size_t handled = size * nmemb;
	twit->buffer.append(data, handled);
	return handled;
}

}