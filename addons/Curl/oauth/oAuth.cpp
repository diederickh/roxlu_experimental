#include "oAuth.h"
#include "Signature.h"
#include "Header.h"

namespace rcp = roxlu::curl::parameter;
namespace rco = roxlu::curl::oauth;
namespace rc = roxlu::curl;

namespace roxlu {
namespace curl {
namespace oauth {

oAuth::oAuth() {
}

void oAuth::updateNonce() {
	char buf_time[512];
	char buf_rand[512];
	memset(buf_time, 0, 512);
	memset(buf_rand, 0, 512);
	srand(time(NULL));
	sprintf(buf_time, "%ld", time(NULL));
	sprintf(buf_rand, "%x", rand()%1000);
	nonce.assign(buf_time);
	nonce.append(buf_rand);
	timestamp.assign(buf_time);
}


/**
 * This method will generate a Request object that can be used to 
 * get a request-token; the first step in the oauth flow.
 *
 * @param	string			The URL where we can get the request-token
 * @param	string			The callback to which the request token is sent.
 * @return	Request			Request object for token-request
 */
rc::Request oAuth::getRequestTokenRequest(const string& url, const string& callbackURL, int authMethod) {
	updateNonce();

	rcp::Collection col = getDefaultParameters();
	col["oauth_callback"] = callbackURL; // out of bound (we only focus on pin!)
	
	// generate the signature. (still w/o access token) 
	string signature = rco::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;
	
	
	rc::Request req;
	req.setURL(url);
	
	if(authMethod == OAUTH_METHOD_GET) {
		req.addParams(col);
		req.isGet(true);
	}
	else if(authMethod == OAUTH_METHOD_HEADER) {
		string header = "Authorization: " +rco::Header::getHeader(col);
		req.addHeader(header);
	}
	
	return req;
}


rc::Request oAuth::getAccessTokenRequest(const string& url, int authMethod) {
	updateNonce();
	
	// get the parameters necessary for a token-request.
	rcp::Collection col = getDefaultParameters();
	col["oauth_token"] = getTokenKey();
	col["oauth_verifier"] = getVerifier();
	
	string signature = rco::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;

	rc::Request req;	
	
	if(authMethod == OAUTH_METHOD_GET) {
		req.addParams(col);
		req.isGet(true);
	}
	else if(authMethod == OAUTH_METHOD_HEADER) {
		string header = "Authorization: " +rco::Header::getHeader(col);
		req.addHeader(header);
	}
	
	req.setURL(url);
	
	return req;

}

void oAuth::authorize(rc::Request& req, int authMethod) {
	updateNonce();
	
	if(authMethod == OAUTH_METHOD_GET) {
		rcp::Collection auth_params;
		getAuthorizeParams(req, req.getParams(), auth_params);
		req.addParams(auth_params);
	}
	else if(authMethod == OAUTH_METHOD_HEADER) {
		string s = getAuthorizationHeader(req, req.getParams());
		req.addHeader(getAuthorizationHeader(req, req.getParams()));
	}
}


string oAuth::getAuthorizationHeader(rc::Request& req, const rcp::Collection& params) {
	return "Authorization: " +getHeader(req, params);
}

string oAuth::getHeader(rc::Request& req, const rcp::Collection& params) {
	rcp::Collection header_params;
	if(getAuthorizeParams(req, params, header_params)) {
		return rco::Header::getHeader(header_params);
	}
	return "";
}

bool oAuth::getAuthorizeParams(rc::Request& req, const rcp::Collection& paramsIn, rcp::Collection& paramsOut) {
	// create collection for signature
	rcp::Collection signature_params = getDefaultParameters();
	
	// only add post or get parameters to the signature params when it's not 
	// a multi part form. See: https://dev.twitter.com/discussions/1059
	if(!req.isMultiPart()) {
		signature_params += paramsIn;
	}
	
	signature_params["oauth_token"] = getTokenKey();
	string signature;
	if(req.isGet()) {
		signature = rco::Signature::getSignatureForGet(*this, req.getURL(),signature_params);
	}
	else if(req.isPost()) {
		signature = rco::Signature::getSignatureForPost(*this, req.getURL(),signature_params);
	}

	// Create header with signature	
	paramsOut = getDefaultParameters();
	paramsOut["oauth_signature"] = signature;
	paramsOut["oauth_token"] = getTokenKey();
	return true;
}


rcp::Collection oAuth::getDefaultParameters() {
	rcp::Collection col;
	col["oauth_consumer_key"] = getConsumerKey();
	col["oauth_signature_method"] = getSignatureMethod();
	col["oauth_timestamp"] = timestamp;
	col["oauth_nonce"] = nonce;
	col["oauth_version"] = getVersion();
	return col;
}

}}} // roxlu::curl::oauth
