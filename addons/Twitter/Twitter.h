#ifndef ROXLU_TWITTERH
#define ROXLU_TWITTERH

#include <vector>
#include <string>
#include <fstream>

#include "TwitteroAuth.h"
#include "../../libs/curl/curl.h"
#include "../../libs/crypto/urlencode.h"
#include "types/TwitterCurlValues.h"
#include "types/TwitterCurlValueType.h"
#include "types/TwitterCurlValueTypeString.h"
#include "types/TwitterCurlValueTypeFile.h"
#include "types/TwitterRequest.h"


#include "parameter/Collection.h"
#include "parameter/Parameter.h"
#include "curl/Request.h"
#include "curl/Curl.h"
#include "oauth/Utils.h"
#include "oauth/oAuth.h"

namespace rtc = roxlu::twitter::curl;
namespace rtp = roxlu::twitter::parameter;
namespace rto = roxlu::twitter::oauth;

using std::string;
using std::vector;


// This implementation is based on twitcurl with some modifications for OF
// All hard work goes to the author of TwitCurl!

namespace roxlu {

class Twitter {
public:
	Twitter();
	~Twitter();
	
//	enum ImageType {
//		 JPG 
//		,PNG
//		,GIF
//	};
	
	//TwitteroAuth& getoAuth();
	bool requestToken(string& authURL /* out */);
	bool accessToken();
	bool handlePin(const string& authURL); 
	bool setPin(const string& pin);
	
	string& getTwitterUsername();
	string& getTwitterPassword();
	void setTwitterUsername(const string& username);
	void setTwitterPassword(const string& password);
	
	void setConsumerKey(const string& consumerKey);
	void setConsumerSecret(const string& consumerSecret);
	
	bool isCurlInitialized();
	bool saveTokens(const string& filePath);
	bool loadTokens(const string& filePath);
	bool removeTokens(const string& filePath);
	
	// API implementation
	bool getHomeTimeline(unsigned int count = 20);
/*
	bool getMentions();
	bool getPublicTimeline();
	bool getRetweetedByMe();
	bool getRetweetedToMe();
	bool getRetweetsOfMe();
	bool getUserTimeline();
	*/
	bool statusUpdate(const string& tweet);
	bool statusUpdateWithMedia(const string& tweet, const string& imageFilePath);
	
	string buffer;
private:
	bool performGet(const string& url);
	bool performGet(const string& url, const string& oAuthHeader);
	bool performGet(const string& url, const vector<TwitterCurlValueType*>& keyValues);
	bool performPost(const string& url, const vector<TwitterCurlValueType*>& keyValues);
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
	
	rto::oAuth oauth;
	rtc::Curl twitcurl;
};


inline void Twitter::setTwitterUsername(const string& username) {
	twitcurl.setAuthUsername(username);
	twitter_username = username;
}

inline void Twitter::setTwitterPassword(const string& password) {
	twitcurl.setAuthPassword(password);
	twitter_password = password;
}

inline string& Twitter::getTwitterUsername() {
	return twitter_username;
}

inline string& Twitter::getTwitterPassword() {
	return twitter_password;
}

inline void Twitter::setConsumerKey(const string& key) {
	auth.setConsumerKey(key);
	oauth.setConsumerKey(key);
}

inline void Twitter::setConsumerSecret(const string& secret) {
	auth.setConsumerSecret(secret);
	oauth.setConsumerSecret(secret);
}
	
inline bool Twitter::isCurlInitialized() {
	return curl != NULL;
}	
	
}; // roxlu

#endif
