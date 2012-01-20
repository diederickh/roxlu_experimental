#ifndef ROXLU_TWITTER_OAUTH_SIGNATUREH
#define ROXLU_TWITTER_OAUTH_SIGNATUREH

#include <string>
#include <list>
#include "../../../libs/crypto/urlencode.h"
#include "../../../libs/crypto/HMAC_SHA1.h"
#include "../../../libs/crypto/base64.h"
#include "../parameter/Collection.h"

using std::string;
using std::list;

namespace rtp = roxlu::twitter::parameter;

namespace roxlu {
namespace twitter {
namespace oauth {

class oAuth;

class Signature {
public:
	//Signature(oAuth& a);
	static string getSignatureForGet(oAuth& auth, const string& url, const rtp::Collection& params);
	static string getSignatureForPost(oAuth& auth, const string& url, const rtp::Collection& params);
	static string getSignature(oAuth& auth, const string& url, const string& method, const rtp::Collection& params);
	static string getParameterStringForSignatureBase(const rtp::Collection& params);
	
	/*
	void addDefaultParameters();
	void addSignatureParameterForGet(const string& url);
	void addSignatureParameterForPost(const string& url);
	void addPinParameter(const string& pin);
	void addTokenParameter(const string& token);
	void addParameter(string name, string value); // unencoded!!
	string getSignatureForGet(const string& url);
	string getSignatureForPost(const string& url);
	string getSignature(const string& url, const string& method);
	string getParameterStringForSignatureBase();
	const map<string, string>& getParameters();
	*/
private:
//	oAuth& auth;
};


}}} // roxlu::twitter::oauth
#endif