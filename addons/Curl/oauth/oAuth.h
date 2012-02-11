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


enum oAuthAuthorizationMethods {
	 OAUTH_METHOD_HEADER 		// add authorization to header
	,OAUTH_METHOD_GET			// add authorization to request vars
};


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
	void setVerifier(const string& verifier); // i.e. the pin from twitter

	void updateNonce();

	string& getConsumerKey();
	string& getConsumerSecret();
	string& getTokenKey();
	string& getTokenSecret();
	string& getScreenName();
	string& getVerifier();
	string& getNonce();
	string& getTimestamp();
	string getSignatureMethod();
	string getVersion();

	string consumer_key;
	string consumer_secret;
	string token_key;
	string token_secret;
	string screen_name;
	string verifier;
	string nonce;
	string timestamp;
	
	// requests
	rc::Request getRequestTokenRequest(const string& url, const string& callbackURL, int authMethod = OAUTH_METHOD_HEADER);
	rc::Request getAccessTokenRequest(const string& url, int authMethod = OAUTH_METHOD_HEADER);

	void authorize(rc::Request& req, int authMethod = OAUTH_METHOD_HEADER);
	bool getAuthorizeParams(rc::Request& req, const rcp::Collection& paramsIn, rcp::Collection& paramsOut);
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

inline string& oAuth::getVerifier() {
	return verifier;
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

inline void oAuth::setVerifier(const string& p) {
	verifier  = p;
}



}}} // roxlu::curl::oauth

#endif