#ifndef ROXLU_TWITTERH
#define ROXLU_TWITTERH

#include <vector>
#include <string>
#include <fstream>

#include "../../libs/curl/curl.h"
#include "../../libs/crypto/urlencode.h"

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

const string URL_STATUSES_BASE = "http://api.twitter.com/1/statuses/";
const string URL_STATUSES_RETWEETED_BY = "/retweeted_by.json";
const string URL_STATUSES_RETWEETED_BY_IDS = "/retweeted_by/ids.json";
const string URL_STATUSES_RETWEETS = URL_STATUSES_BASE +"retweets/";
const string URL_STATUSES_SHOW = URL_STATUSES_BASE +"show.json";
const string URL_STATUSES_DESTROY = URL_STATUSES_BASE +"destroy/";
const string URL_STATUSES_RETWEET = URL_STATUSES_BASE +"retweet/";
const string URL_STATUSES_UPDATE = URL_STATUSES_BASE +"update.json";
const string URL_STATUSES_UPDATE_WITH_MEDIA = URL_STATUSES_BASE +"update_with_media.json";
const string URL_STATUSES_OEMBED = URL_STATUSES_BASE +"oembed.json";

const string URL_STATUSES_HOME_TIMELINE = URL_STATUSES_BASE +"home_timeline.json";
const string URL_STATUSES_MENTIONS = URL_STATUSES_BASE +"mentions.json";
const string URL_STATUSES_PUBLIC_TIMELINE = URL_STATUSES_BASE +"public_timeline.json";
const string URL_STATUSES_RETWEETED_BY_ME = URL_STATUSES_BASE +"retweeted_by_me.json";
const string URL_STATUSES_RETWEETED_TO_ME = URL_STATUSES_BASE +"retweeted_to_me.json";
const string URL_STATUSES_RETWEETS_OF_ME = URL_STATUSES_BASE +"retweets_of_me.json";
const string URL_STATUSES_USER_TIMELINE = URL_STATUSES_BASE +"user_timeline.json";
const string URL_STATUSES_RETWEETED_TO_USER = URL_STATUSES_BASE +"retweeted_to_user.json";
const string URL_STATUSES_RETWEETED_BY_USER = URL_STATUSES_BASE +"retweeted_by_user.json";

const string URL_SEARCH = "http://search.twitter.com/search.json";

const string URL_DIRECT_MESSAGES = "http://api.twitter.com/1/direct_messages.json";

//const string URL_STATUSES_MENTIONS = URL_STATUSES_BASE +



const string URL_AUTHORIZE = "http://twitter.com/oauth/authorize?oauth_token=";


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
	
	// API: Tweets
	bool statusesRetweetedBy(const string& tweetID, rtp::Collection* extraParams =  NULL);
	bool statusesRetweetedByIDs(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesRetweets(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesShow(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesDestroy(const string& tweetID, rtp::Collection* extraParams = NULL); 
	bool statusesRetweet(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesUpdate(const string& tweet, rtp::Collection* extraParams =  NULL);
	bool statusesUpdateWithMedia(const string& tweet, const string& imageFilePath, rtp::Collection* extraParams =  NULL);
	bool statusesoEmbed(const string& tweetID, rtp::Collection* extraParams =  NULL); 
	
	
	// API: Timelines
	bool statusesHomeTimeline(rtp::Collection* extraParams =  NULL);
	bool statusesMentions(rtp::Collection* extraParams =  NULL);
	bool statusesPublicTimeline(rtp::Collection* extraParams =  NULL);
	bool statusesRetweetedByMe(rtp::Collection* extraParams = NULL);
	bool statusesRetweetedToMe(rtp::Collection* extraParams = NULL);
	bool statusesRetweetsOfMe(rtp::Collection* extraParams = NULL);
	bool statusesUserTimeline(rtp::Collection* extraParams = NULL);
	bool statusesRetweetedToUser(const string& screenName, rtp::Collection* extraParams = NULL);
	bool statusesRetweetedByUser(const string& screenName, rtp::Collection* extraParams = NULL);
	
	// API: search
	bool search(const string& query, rtp::Collection* extraParams = NULL);
	bool directMessages(rtp::Collection* extraParams = NULL);
	// ++
	
	
	

	
	
/*
	bool getMentions();
	bool getPublicTimeline();
	bool getRetweetedByMe();
	bool getRetweetedToMe();
	bool getRetweetsOfMe();
	bool getUserTimeline();
	*/
	string& getResponse();	
	
private:
	bool doGet(const string& url, rtp::Collection* defaultParams = NULL, rtp::Collection* extraParams = NULL);
	bool doPost(const string& url, const rtp::Collection& col, bool multiPart = false, rtp::Collection* extraParams = NULL);
	bool doPost(const string& url, bool multiPart = false, rtp::Collection* extraParams = NULL);
	void reset();
	string response;
	rto::oAuth oauth;
	rtc::Curl twitcurl;
};

inline bool Twitter::doGet(const string& url, rtp::Collection* defaultParams, rtp::Collection* extraParams) {
	reset();
	rtc::Request req(url);
	req.isGet(true);
	
	if(defaultParams != NULL) {
		req.getParams() += *defaultParams;
	}
		
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	
	
	oauth.authorize(req);
	return req.doGet(twitcurl, response);
}


inline bool Twitter::doPost(const string& url, bool multiPart, rtp::Collection* extraParams) {
	reset();
	rtc::Request req(url);
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response, multiPart);

}

inline bool Twitter::doPost(const string& url, const rtp::Collection& col, bool multiPart, rtp::Collection* extraParams) {	
	reset();
	rtc::Request req(url);
	req.addParams(col);
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response, multiPart);
}




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
