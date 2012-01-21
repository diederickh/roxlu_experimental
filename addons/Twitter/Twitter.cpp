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

bool Twitter::statusesUpdate(const string& tweet) {
	reset();
	rtc::Request req(URL_STATUSES_UPDATE);
	req.getParams().addString("status", tweet);
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response);
}


bool Twitter::statusesUpdateWithMedia(const string& tweet, const string& imageFilePath) {
	reset();
	rtc::Request req(URL_STATUSES_UPDATE_WITH_MEDIA);
	req.getParams().addString("status", tweet);
	req.getParams().addFile("media[]", imageFilePath);
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response, true);
}

// https://dev.twitter.com/docs/api/1/get/statuses/home_timeline
bool Twitter::statusesHomeTimeline() {
	return doGet(URL_HOME_TIMELINE);
}

// https://dev.twitter.com/docs/api/1/get/statuses/public_timeline
bool Twitter::statusesPublicTimeline() {
	return doGet(URL_PUBLIC_TIMELINE);
}

// https://dev.twitter.com/docs/api/1/get/statuses/mentions
bool Twitter::statusesMentions() {
	return doGet(URL_STATUSES_MENTIONS);
}


}} // roxlu::twitter