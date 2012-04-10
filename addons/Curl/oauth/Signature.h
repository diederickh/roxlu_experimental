#ifndef ROXLU_TWITTER_OAUTH_SIGNATUREH
#define ROXLU_TWITTER_OAUTH_SIGNATUREH

#include <string>
#include <list>
#include "../../../external/includes/crypto/urlencode.h"
#include "../../../external/includes/crypto/HMAC_SHA1.h"
#include "../../../external/includes/crypto/base64.h"
#include "../parameter/Collection.h"
#include "../parameter/Parameter.h"

using std::string;
using std::list;

namespace rcp = roxlu::curl::parameter;

namespace roxlu {
namespace curl {
namespace oauth {

class oAuth;

class Signature {
public:
	static string getSignatureForGet(oAuth& auth, const string& url, const rcp::Collection& params);
	static string getSignatureForPost(oAuth& auth, const string& url, const rcp::Collection& params);
	static string getSignature(oAuth& auth, const string& url, const string& method, const rcp::Collection& params);
	static string getParameterStringForSignatureBase(const rcp::Collection& params);

private:

};


}}} // roxlu::curl::oauth
#endif