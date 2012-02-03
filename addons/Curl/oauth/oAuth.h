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
#include "../Request.h"


using std::string;
using std::map;
using std::string;
using std::list;
using std::vector;

namespace rcp = roxlu::curl::parameter;
namespace rc = roxlu::curl;

namespace roxlu {
namespace curl {
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
	rc::Request getRequestTokenRequest(const string& url, const string& callbackURL);
	rc::Request getAccessTokenRequest(const string& url);

	void authorize(rc::Request& req);
	string getHeader(rc::Request& req, const rcp::Collection& params);
	string getAuthorizationHeader(rc::Request& req, const rcp::Collection& params);
	
	// collections 
	rcp::Collection getDefaultParameters(); // private?
	

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



}}} // roxlu::curl::oauth

#endif