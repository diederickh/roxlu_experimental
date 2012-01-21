#ifndef ROXLU_TWITTER_OAUT_OAUTH
#define ROXLU_TWITTER_OAUT_OAUTH

#include <string>
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <map>

#include "../parameter/Collection.h"
#include "../parameter/Parameter.h"
#include "../curl/Request.h"


using std::string;
using std::map;
using std::string;
using std::list;
using std::vector;

namespace rtp = roxlu::twitter::parameter;
namespace rtc = roxlu::twitter::curl;

namespace roxlu {
namespace twitter {
namespace oauth {

class oAuth {
public:		
	oAuth();
	
	void setConsumerKey(const string& key);
	void setConsumerSecret(const string& secret);
	void setTokenKey(const string& key);
	void setTokenSecret(const string& secret);
	void setScreenName(const string& name);
	void setPin(const string& p);

	void updateNonce();

	string& getConsumerKey();
	string& getConsumerSecret();
	string& getTokenKey();
	string& getTokenSecret();
	string& getScreenName();
	string& getPin();
	string& getNonce();
	string& getTimestamp();
	string getSignatureMethod();
	string getVersion();

	string consumer_key;
	string consumer_secret;
	string token_key;
	string token_secret;
	string screen_name;
	string pin;
	string nonce;
	string timestamp;
	
	// requests
	rtc::Request getRequestTokenRequest();
	rtc::Request getAccessTokenRequest();

	// @todo check if getAuthorizedPost or getAuthorizedGet are still used
	// @todo I think only authorize is necessary
	rtc::Request getAuthorizedPost(const string& url, const rtp::Collection& params);
	rtc::Request getAuthorizedGet(const string& url, const rtp::Collection& params);
	void authorize(rtc::Request& req);
	
	// collections 
	rtp::Collection getDefaultParameters(); // private?
	

private:

};

// get.
// -----------------------------------------------------------------------------
inline string& oAuth::getConsumerKey() {
	return consumer_key;
}

inline string& oAuth::getConsumerSecret() {
	return consumer_secret;
}

inline string& oAuth::getTokenKey() {
	return token_key;
}

inline string& oAuth::getTokenSecret() {
	return token_secret;
}


inline string& oAuth::getScreenName() {
	return screen_name;
}

inline string& oAuth::getPin() {
	return pin;
}

inline string& oAuth::getNonce() {
	return nonce;
}

inline string& oAuth::getTimestamp() {
	return timestamp;
}

inline string oAuth::getSignatureMethod() {
	return "HMAC-SHA1";
}

inline string oAuth::getVersion() {
	return "1.0";
}

// set.
// -----------------------------------------------------------------------------
inline void oAuth::setConsumerKey(const string& k) {
	consumer_key = k;
}

inline void oAuth::setConsumerSecret(const string& s) {
	consumer_secret = s;
}	

inline void oAuth::setTokenKey(const string& k) {
	token_key = k;
}

inline void oAuth::setTokenSecret(const string& s) {
	token_secret = s;
}

inline void oAuth::setScreenName(const string& n) {
	screen_name = n;
}

inline void oAuth::setPin(const string& p) {
	pin  = p;
}



}}} // roxlu::twitter::oauth

#endif