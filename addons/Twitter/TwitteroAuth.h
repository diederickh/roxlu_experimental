#ifndef ROXLU_TWITTEROAUTHH
#define ROXLU_TWITTEROAUTHH

// Based on TwitCurl

#include <string>
#include <map>
#include <list>
#include <cstdlib>
#include <map>

using std::map;
using std::string;
using std::list;

namespace roxlu {


namespace twitter {
    const string CONSUMERKEY_KEY						= "oauth_consumer_key";
    const string CALLBACK_KEY							= "oauth_callback";
    const string VERSION_KEY							= "oauth_version";
    const string SIGNATUREMETHOD_KEY					= "oauth_signature_method";
    const string SIGNATURE_KEY							= "oauth_signature";
    const string TIMESTAMP_KEY							= "oauth_timestamp";
    const string NONCE_KEY								= "oauth_nonce";
    const string TOKEN_KEY								= "oauth_token";
    const string TOKENSECRET_KEY						= "oauth_token_secret";
    const string VERIFIER_KEY							= "oauth_verifier";
    const string SCREENNAME_KEY							= "screen_name";
    const string AUTHENTICITY_TOKEN_KEY					= "authenticity_token";
    const string SESSIONUSERNAME_KEY					= "session[username_or_email]";
    const string SESSIONPASSWORD_KEY					= "session[password]";
    const string AUTHENTICITY_TOKEN_TWITTER_RESP_KEY	= "authenticity_token\" type=\"hidden\" value=\"";
    const string TOKEN_TWITTER_RESP_KEY					= "oauth_token\" type=\"hidden\" value=\"";
    const string PIN_TWITTER_RESP_KEY					= "code-desc\"><code>";
    const string TOKEN_END_TAG_TWITTER_RESP				= "\" />";
    const string PIN_END_TAG_TWITTER_RESP				= "</code>";
    const string AUTHHEADER_STRING						= "Authorization: OAuth ";
	
    const string REQUEST_TOKEN_URL 	= "http://twitter.com/oauth/request_token";
    const string AUTHORIZE_URL 		= "http://twitter.com/oauth/authorize?oauth_token=";
    const string ACCESS_TOKEN_URL 	= "http://twitter.com/oauth/access_token";
};


class TwitteroAuth {
public:
	enum RequestType {
		 TWITTER_OAUTH_GET
		,TWITTER_OAUTH_POST
		,TWITTER_OAUTH_DELETE
	};
	
	string& getConsumerKey();
	string& getConsumerSecret();
	string& getTokenKey();
	string& getTokenSecret();
	string& getScreenName();
	string& getPin();
	
	void setConsumerKey(const string& key);
	void setConsumerSecret(const string& secret);
	void setTokenKey(const string& key);
	void setTokenSecret(const string& secret);
	void setScreenName(const string& name);
	void setPin(const string& p);

	bool getHeader(
				 const RequestType type
				,const string url
				,const string data
				,string& header /* out */
				,const bool includePin = false
			);
	
	bool extractTokenKeyAndSecret(const string& buffer);
	

private:
	bool buildoAuthTokenKeyValuePairs(
				 const bool includePin
				,const string& data
				,const string& signature
				,map<string, string>& keyValues
				,const bool generateTimestamp
			);
	
	bool getSignature(
				 const RequestType type
				,const string& url
				,const map<string, string>& keyValues
				,string& signature /* out */
			);
			
	bool getStringFromKeyValuePairs(
				 const map<string, string>& keyValues
				,string& params /* out */
				,const string& paramSep
			);

	void updateNonce();

	string consumer_key;
	string consumer_secret;
	string token_key;
	string token_secret;
	string screen_name;
	string pin;
	string nonce;
	string timestamp;
}; // TwitteroAuth

// get.
// -----------------------------------------------------------------------------
inline string& TwitteroAuth::getConsumerKey() {
	return consumer_key;
}

inline string& TwitteroAuth::getConsumerSecret() {
	return consumer_secret;
}

inline string& TwitteroAuth::getTokenKey() {
	return token_key;
}

inline string& TwitteroAuth::getTokenSecret() {
	return token_secret;
}


inline string& TwitteroAuth::getScreenName() {
	return screen_name;
}

inline string& TwitteroAuth::getPin() {
	return pin;
}

// set.
// -----------------------------------------------------------------------------
inline void TwitteroAuth::setConsumerKey(const string& k) {
	consumer_key = k;
}

inline void TwitteroAuth::setConsumerSecret(const string& s) {
	consumer_secret = s;
}	

inline void TwitteroAuth::setTokenKey(const string& k) {
	token_key = k;
}

inline void TwitteroAuth::setTokenSecret(const string& s) {
	token_secret = s;
}

inline void TwitteroAuth::setScreenName(const string& n) {
	screen_name = n;
}

inline void TwitteroAuth::setPin(const string& p) {
	pin  = p;
}

}; // roxlu

#endif