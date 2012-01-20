#ifndef ROXLU_TWITTER_OAUTH_UTILSH
#define ROXLU_TWITTER_OAUTH_UTILSH

// collection of utils used by oauth; mostly just string manipulation functions
#include <string>

using std::string;

namespace roxlu {
namespace twitter {
namespace oauth {

const string TOKEN_KEY				= "oauth_token";
const string TOKEN_SECRET_KEY		= "oauth_token_secret";
const string AUTHENTICITY_ATTRIBUTE	= "authenticity_token\" type=\"hidden\" value=\"";
const string AUTHENTICITY_END_TAG	= "\" />";

const string PIN_TAG				= "code-desc\"><code>";
const string PIN_END_TAG			= "</code>";


class Utils {
public:
	static bool extractTokenAndSecret(const string& buffer, string& token, string& secret);	
	static bool extractAuthenticityToken(const string& buffer, string& authenticity);
	static bool extractPin(const string& buffer, string& pin);
};

}}} // roxlu::twitter::oauth

#endif