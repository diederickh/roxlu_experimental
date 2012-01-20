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
namespace twitter {

const string URL_STATUS_UPDATE = "http://api.twitter.com/1/statuses/update.json";
const string URL_STATUS_WITH_MEDIA_UPDATE = "https://upload.twitter.com/1/statuses/update_with_media.json";
const string URL_AUTHORIZE = "http://twitter.com/oauth/authorize?oauth_token=";
const string URL_HOME_TIMELINE = "http://api.twitter.com/1/statuses/home_timeline.json";

class Twitter {
public:
	Twitter();
	~Twitter();

	bool requestToken(string& authURL /* out */);
	bool accessToken();
	bool handlePin(const string& authURL); 
	bool setPin(const string& pin);
	
	void setTwitterUsername(const string& username);
	void setTwitterPassword(const string& password);
	
	void setConsumerKey(const string& consumerKey);
	void setConsumerSecret(const string& consumerSecret);
	
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
	string& getResponse();	
	
private:
	string response;
	rto::oAuth oauth;
	rtc::Curl twitcurl;
};


inline void Twitter::setTwitterUsername(const string& username) {
	twitcurl.setAuthUsername(username);
}

inline void Twitter::setTwitterPassword(const string& password) {
	twitcurl.setAuthPassword(password);
}

inline void Twitter::setConsumerKey(const string& key) {
	oauth.setConsumerKey(key);
}

inline void Twitter::setConsumerSecret(const string& secret) {
	oauth.setConsumerSecret(secret);
}

inline string& Twitter::getResponse() {
	return response;
}
	
}} // roxlu twitter

#endif
