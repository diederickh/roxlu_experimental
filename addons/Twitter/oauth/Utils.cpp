#include "Utils.h"

namespace roxlu {
namespace twitter {
namespace oauth {


/**
 * When you do a request for a request-token you get a string back 
 * which contains the token and token-secret. This function will parse
 * this string and sets the in parameters token and secrect
 *
 * @param	string	[in]	The buffer. Like:
 *
 *						oauth_token=KYXJW4x6EB67bxpRYKTelcE8Hc9rOAyosxF0UtWEds \
 *						&oauth_token_secret=OXPe2Sebc1PKrKUlOJn4IYLIngQhQnAy6n \
 *						OYmYm88&oauth_callback_confirmed=true 
 * 
 * @param	string [out]	We set oauth_token value to this variable
 * @param	string [out]	We set oauth_token_secret to this variable
 *
 */
bool Utils::extractTokenAndSecret(
	 const string& buffer
	,string& token
	,string& secret
)
{
	if(!buffer.length()) {
		return false;
	}
	
	// find oauth_token value
	size_t npos = string::npos;
	string dummy;
	npos = buffer.find(TOKEN_KEY);
	if(npos != string::npos) {
		npos = npos +TOKEN_KEY.length()+1;
		dummy = buffer.substr(npos);
		npos = dummy.find("&");
		if(npos != string::npos) {
			token = dummy.substr(0,npos);
		}
	}
	
	// find oauth_token_secret
	npos = buffer.find(TOKEN_SECRET_KEY);
	if(npos != string::npos) {
		npos = npos +TOKEN_SECRET_KEY.length() +1;
		dummy = buffer.substr(npos);
		npos = dummy.find("&");
		if(npos != string::npos) {
			secret = dummy.substr(0, npos);
		}
	}	
	//printf("Tokenkey: '%s', Token secret: '%s'\n", token_key.c_str(), token_secret.c_str()); 	
	return true;

}

/**  
 * When using the PIN based authorization, the user needs to enter a PIN. 
 * The Twitter API implementation (Twitter class), uses this function to extract
 * the pin and simulate a webpost to twitter again to authorize the application
 * 
 * @param	string [in]		The buffer (the HTML with the pincode)
 * @param	string [out]	We set the pin value to this.
 */
bool Utils::extractAuthenticityToken(const string& buffer, string& authenticity) {
	// get: <input name="authenticity_token" type="hidden" value="11187c853df9ca61369472e70f5652ddc2922333" />
	size_t npos_start;
	size_t npos_end;
	
	npos_start = buffer.find(AUTHENTICITY_ATTRIBUTE);
	npos_start += AUTHENTICITY_ATTRIBUTE.length();
	npos_end = buffer.substr(npos_start).find(AUTHENTICITY_END_TAG);
	authenticity = buffer.substr(npos_start, npos_end);
	return authenticity.length();
}

/**
 * Part of the automatically authorizing PIN flow. We parse the HTML of twitter
 * and extract the PIN code, then submit this as it was the use who entered the
 * pin.
 *
 * @param	string [in]		The HTML from twitter containing the PIN 
 * @param	string [out]	Is set to the extract pin.
 */
bool Utils::extractPin(const string& buffer, string& pin) {
	size_t npos_start;
	size_t npos_end;

	npos_start = buffer.find(PIN_TAG);
	npos_start += PIN_TAG.length();
	npos_end = buffer.substr(npos_start).find(PIN_END_TAG);
	pin = buffer.substr(npos_start, npos_end);
	return pin.length();
}

}}} // roxlu::twitter::oauth