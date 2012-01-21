#include <algorithm>
#include "Twitter.h"

namespace roxlu {
namespace twitter {

Twitter::Twitter() {
}

Twitter::~Twitter() {
}

bool Twitter::requestToken(string& authURL /* out */) {
	// get request-token.
	string response;
	rtc::Request req = oauth.getRequestTokenRequest();
	if(!req.doGet(twitcurl, response)) {
		printf("error: cannot do request for token.\n");
		return false;
	}
	
	// extract request-token and secret from result.
	string token;
	string secret;
	if(!rto::Utils::extractTokenAndSecret(response, token, secret)) {
		return false;
	}
	oauth.setTokenKey(token);
	oauth.setTokenSecret(secret);
	//printf("token: %s secret: %s\n", token.c_str(), secret.c_str());
	// use these token and secret to create the authorization url.
	authURL.assign(URL_AUTHORIZE);
	authURL.append(oauth.getTokenKey());
	return true;
}

// Get pin which authorizes the application.
// -----------------------------------------
bool Twitter::handlePin(const string& authURL) {

	// STEP 1: get PIN-wise authorization
	// -------------------------------------------------------------------------
	string response;
	rtc::Request req;
	req.setURL(authURL);
	if(!req.doGet(twitcurl, response)) {
		printf("error: cannot do request for token.\n");
		return false;
	}
	
	// get authenticity token from authorization page. 
	string authenticity_token;
	rto::Utils::extractAuthenticityToken(response, authenticity_token);
	
	// STEP 2: do a post with the fields on this pin auth page.
	// -------------------------------------------------------------------------
	req.getParams().addString("oauth_token", oauth.getTokenKey());
	req.getParams().addString("authenticity_token", authenticity_token);
	req.getParams().addString("session[username_or_email]", twitcurl.getAuthUsername());
	req.getParams().addString("session[password]", twitcurl.getAuthPassword());
	
	if(!req.doPost(twitcurl, response)) {
		printf("error: cannot get pin html\n");
		return false;
	}
	
	string pin;
	if(!rto::Utils::extractPin(response, pin)) {	
		printf("error: cannot extract pin.\n");
		return false;
	}
	oauth.setPin(pin);
	return true;
}

bool Twitter::accessToken() {
	// get request with correct headers (and signature)
	string response; 
	rtc::Request req = oauth.getAccessTokenRequest();
	if(!req.doGet(twitcurl, response)) {
		return false;
	}
	
	// extract token and secrect from response.
	string token;
	string secret;
	if(!rto::Utils::extractTokenAndSecret(response, token, secret)) {
		return false;
	}
	oauth.setTokenKey(token);
	oauth.setTokenSecret(secret);
	return true;
}

// Save retrieved tokens which gives the application access to the twitter account
// -------------------------------------------------------------------------------
bool Twitter::saveTokens(const string& filePath) {
	std::ofstream of(filePath.c_str());
	if(!of.is_open()) {
		printf("Error: cannot open: '%s'.\n", filePath.c_str());
		return false;
	}
	of << oauth.getTokenKey() << std::endl;
	of << oauth.getTokenSecret() << std::endl;
	of.close();
	return true;
}

bool Twitter::loadTokens(const string& filePath) {
	std::ifstream ifs(filePath.c_str());
	if(!ifs.is_open()) {
		printf("Error: cannot open: '%s'.\n", filePath.c_str());
		return false;
	}
	string token_key, token_secret;
	std::getline(ifs, token_key);
	std::getline(ifs, token_secret);
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);
	return true;
}

bool Twitter::removeTokens(const string& filePath) {
	std::remove(filePath.c_str());
}

// API implementation
// -----------------------------------------------------------------------------
// called just before a new API request is made.
void Twitter::reset() {
	response.clear(); 
}

// :::::::::::::::: tweets

// https://dev.twitter.com/docs/api/1/get/statuses/%3Aid/retweeted_by
bool Twitter::statusesRetweetedBy(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_BASE +tweetID +URL_STATUSES_RETWEETED_BY;
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/%3Aid/retweeted_by/ids
bool Twitter::statusesRetweetedByIDs(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_BASE +tweetID +URL_STATUSES_RETWEETED_BY_IDS;
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweets/%3Aid
bool Twitter::statusesRetweets(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_RETWEETS +tweetID +".json";
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/show/%3Aid
bool Twitter::statusesShow(const string& tweetID, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("id", tweetID);
	return doGet(URL_STATUSES_SHOW, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/destroy/%3Aid
bool Twitter::statusesDestroy(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_DESTROY +tweetID +".json";
	return doPost(url, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/retweet/%3Aid
bool Twitter::statusesRetweet(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_RETWEET +tweetID +".json";
	return doPost(url, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/update
bool Twitter::statusesUpdate(const string& tweet, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("status", tweet);
	return doPost(URL_STATUSES_UPDATE, col, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/update_with_media
bool Twitter::statusesUpdateWithMedia(const string& tweet, const string& imageFilePath, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("status", tweet);
	col.addFile("media[]", imageFilePath);
	return doPost(URL_STATUSES_UPDATE_WITH_MEDIA, col, true, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/oembed
bool Twitter::statusesoEmbed(const string& tweetID, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("id", tweetID);
	col.addString("url", "https://twitter.com/twitter/status/" +tweetID);
	return doGet(URL_STATUSES_OEMBED, &col, extraParams);
}


// :::::::::::::::: timeslines 

// https://dev.twitter.com/docs/api/1/get/statuses/home_timeline
bool Twitter::statusesHomeTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_HOME_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/mentions
bool Twitter::statusesMentions(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_MENTIONS, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/public_timeline
bool Twitter::statusesPublicTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_PUBLIC_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_by_me
bool Twitter::statusesRetweetedByMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETED_BY_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_to_me
bool Twitter::statusesRetweetedToMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETED_TO_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweets_of_me
bool Twitter::statusesRetweetsOfMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETS_OF_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/user_timeline
bool Twitter::statusesUserTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_USER_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_to_user
bool Twitter::statusesRetweetedToUser(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_STATUSES_RETWEETED_TO_USER, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_by_user
bool Twitter::statusesRetweetedByUser(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_STATUSES_RETWEETED_BY_USER, &col, extraParams);
}

// :::::::::::::::: search
bool Twitter::search(const string& query, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("q", query);
	return doGet(URL_SEARCH, &col, extraParams);
}

// :::::::::::::::: direct mesages
bool Twitter::directMessages(rtp::Collection* extraParams) {
	return doGet(URL_DIRECT_MESSAGES, extraParams);
}







}} // roxlu::twitter