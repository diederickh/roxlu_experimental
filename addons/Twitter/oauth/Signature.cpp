#include "Signature.h"
#include "oAuth.h"
#include "../parameter/Parameter.h"

namespace rtp = roxlu::twitter::parameter;

namespace roxlu {
namespace twitter {
namespace oauth {

class oAuth;
//
//Signature::Signature(oAuth& a) 
//	:auth(a) 
//{
//}


string Signature::getSignatureForGet(oAuth& auth, const string& url, const rtp::Collection& params) {
	return getSignature(auth, url, "GET", params);
}

string Signature::getSignatureForPost(oAuth& auth, const string& url, const rtp::Collection& params) {
	return getSignature(auth, url, "POST", params);
}


/**
 * Creates a signature for oAuth
 * 
 * @param	string		The URL (not urlencoded)
 * @param	string		The method (GET, POST or DELETE) (not urlencoded)
 * @param	Collection	Collection with parameter we use to create the signature
 * @see 	https://dev.twitter.com/docs/auth/creating-signature
 * @see		http://tools.ietf.org/html/rfc5849#section-3.4.1.3.1 <-- some important notes on what to include!
 * @return	string		The signature
 */
string Signature::getSignature(oAuth& auth, const string& url, const string& method, const rtp::Collection& params) {
	// step 1: create signature base string
	// -------------------------------------
	string sig_base;
	sig_base.assign(method);
	sig_base.append("&");
	sig_base.append(urlencode(url));
	sig_base.append("&");
	
	string param_string = getParameterStringForSignatureBase(params);
//	printf("Parameter string: %s\n", param_string.c_str());
	if(param_string.length()) {
		sig_base.append(urlencode(param_string));
	}
	printf("Signature base: %s\n", sig_base.c_str());
	
	// step 2: use signature base string to create signature
	// -----------------------------------------------------
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
	printf("Signature: %s\n", signature.c_str());
	return signature;
}


/**
 * Creates a name=value&name=value&name=value string for the given collection.
 * The values and names in the collection should NOT be urlencoded.
 *
 * @param	Collection		Collection for which we create a paramstring
 * @return	string			Parameter string.
 *
 */
string Signature::getParameterStringForSignatureBase(const rtp::Collection& col) {
	// see https://dev.twitter.com/docs/auth/creating-signature 
	list<string> value_list;
	list<rtp::Parameter*> pars = col.getParameters(true);
	{
		list<rtp::Parameter*>::const_iterator it = pars.begin();
		string varval;
		while(it != pars.end()) {
			varval.assign(urlencode((*it)->getName()));
			varval.append("="); // =
			varval.append(urlencode((*it)->getStringValue()));
			value_list.push_back(varval);
			++it;
		}
	}
	// sort the list (important!) and combine var-values pairs
	value_list.sort();
	string base; 
	{
		list<string>::const_iterator it = value_list.begin();
		while(it != value_list.end()) {
			base.append(*it);
			++it;
			if(it != value_list.end()) {
				base.append("&"); // &
			}
		}
	}
	//printf(">>>>>>>>'%s'<<<<<<<<<<\n", base.c_str());
	return base;
}


}}} // roxlu::twitter::oauth