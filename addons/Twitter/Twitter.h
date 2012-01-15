#ifndef TWITTERH
#define TWITTERH

#include <string>

#include "TwitteroAuth.h"
#include "../../libs/curl/curl.h"

using std::string;

// This implementation is based on twitcurl with some modifications for OF
// All hard work goes to the author of TwitCurl!

namespace roxlu {

class Twitter {
public:
	Twitter();
	~Twitter();
	
	//TwitteroAuth& getoAuth();
	bool oAuthRequestToken(string& authURL /* out */);
	bool oAuthAccesToken();
	bool oAuthHandlePin(const string& authURL);
	
	string& getTwitterUsername();
	string& getTwitterPassword();
	void setTwitterUsername(string& username);
	void setTwitterPassword( string& password);
	
	void oAuthSetConsumerKey(const string& consumerKey);
	void oAuthSetConsumerSecret(const string& consumerSecret);
	
	bool isCurlInitialized();

	
	string buffer;
private:
	bool performGet(const string& url);
	bool performGet(const string& url, const string& oAuthHeader);
	void prepareCurlCallback();
	void prepareCurlUserPass();
	void prepareCurlStandardParams();
	static size_t curlCallback(char* data, size_t size, size_t nmemb, Twitter* twit);
	
	
	bool curl_param_login_set;
	bool curl_param_callback_set;

	CURL* curl;
	
	string twitter_username;
	string twitter_password;
	TwitteroAuth auth;
};


inline void Twitter::setTwitterUsername(string& username) {
	twitter_username = username;
}

inline void Twitter::setTwitterPassword(string& password) {
	twitter_password = password;
}

inline string& Twitter::getTwitterUsername() {
	return twitter_username;
}

inline string& Twitter::getTwitterPassword() {
	return twitter_password;
}

inline void Twitter::oAuthSetConsumerKey(const string& key) {
	auth.setConsumerKey(key);
}

inline void Twitter::oAuthSetConsumerSecret(const string& secret) {
	auth.setConsumerSecret(secret);
}
	
inline bool Twitter::isCurlInitialized() {
	return curl != NULL;
}	
	
}; // roxlu

#endif
