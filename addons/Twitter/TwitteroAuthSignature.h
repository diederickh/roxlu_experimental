#ifndef ROXLU_TWITTEROAUTH_SIGNATUREH
#define ROXLU_TWITTEROAUTH_SIGNATUREH

#include "../../libs/crypto/urlencode.h"
#include "../../libs/crypto/HMAC_SHA1.h"
#include "../../libs/crypto/base64.h"


#include <string>
#include <list>
#include <map>

using std::string;
using std::map;
using std::list;

namespace roxlu {

class TwitteroAuth;

class TwitteroAuthSignature {
public:
	TwitteroAuthSignature(TwitteroAuth& oAuth);
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
private:
	map<string, string> parameters;
	TwitteroAuth& auth;
};

inline const map<string, string>& TwitteroAuthSignature::getParameters() {
	return parameters;
}

};

#endif
