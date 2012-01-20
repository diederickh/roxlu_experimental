#include "TwitteroAuthSignature.h"
#include "TwitteroAuth.h"

namespace roxlu {

TwitteroAuthSignature::TwitteroAuthSignature(TwitteroAuth& oAuth)
	:auth(oAuth)
{

}

void TwitteroAuthSignature::addSignatureParameterForGet(const string& url) {
	addParameter("oauth_signature", getSignatureForGet(url));
}

void TwitteroAuthSignature::addSignatureParameterForPost(const string& url) {
	addParameter("oauth_signature", getSignatureForPost(url));
}

void TwitteroAuthSignature::addPinParameter(const string& pin) {
	addParameter("oauth_verifier", pin);
}

void TwitteroAuthSignature::addTokenParameter(const string& token) {
	addParameter("oauth_token", token);
}

void TwitteroAuthSignature::addDefaultParameters() {
	addParameter("oauth_consumer_key", auth.getConsumerKey());
	addParameter("oauth_nonce", auth.getNonce());
	addParameter("oauth_signature_method", "HMAC-SHA1");
	addParameter("oauth_timestamp", auth.getTimestamp());
	addParameter("oauth_version", "1.0");
}

void TwitteroAuthSignature::addParameter(string name, string value) {
	parameters[name] = value;
}

string TwitteroAuthSignature::getSignatureForGet(const string& url) {
	return getSignature(url, "GET");
}

string TwitteroAuthSignature::getSignatureForPost(const string& url) {
	return getSignature(url, "POST");
}

string TwitteroAuthSignature::getSignature(const string& url, const string& method) {
	// step 1: create signature base string
	// -------------------------------------
	string sig_base;
	sig_base.assign(method);
	sig_base.append("&");
	sig_base.append(urlencode(url));
	sig_base.append("&");
	
	string params = getParameterStringForSignatureBase();
	if(params.length()) {
		sig_base.append(params);
	}
	
	// step 2: use signature base string to create signature
	CHMAC_SHA1 mac;
	string sign_key;
	unsigned char digest[1024];
	memset(digest, 0, 1024);
	
	// create sign key: https://dev.twitter.com/docs/auth/creating-signature
	sign_key.assign(urlencode(auth.getConsumerSecret()));
	sign_key.append("&");
	
	string toksec = auth.getTokenSecret();
	if(toksec.length()) {
		sign_key.append(urlencode(toksec));
	}
	
	// use signature base + sign key to create signature
	mac.HMAC_SHA1(
				(unsigned char*)sig_base.c_str()
				,sig_base.length()
				,(unsigned char*)sign_key.c_str()
				,sign_key.length()
				,digest
			);
			
	string base64_encoded = base64_encode(digest, 20);
	string signature = urlencode(base64_encoded);
	return signature;
}


// creates an encoded var=value&var2=value2 etc.. string.
string TwitteroAuthSignature::getParameterStringForSignatureBase() {
	// see https://dev.twitter.com/docs/auth/creating-signature 
	list<string> value_list;
	{
		map<string, string>::const_iterator it = parameters.begin();
		string varval;
		while(it != parameters.end()) {
			varval.assign(urlencode(it->first));
			varval.append("%3D"); // =
			varval.append(urlencode(it->second));
			value_list.push_back(varval);
			++it;
		}
	}
	
	// sort the list (important!) and combine the var-value pairs.
	value_list.sort();	
	string base;
	{
		list<string>::const_iterator it = value_list.begin();
		while(it != value_list.end()) {
			base.append(*it);
			++it;
			if(it != value_list.end()) {
				base.append("%26"); // &
			}
		}
	}
	return base;
}


}; // roxlu