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
	printf("> %s\n", timestamp.c_str());
}


/**
 * This method will generate a Request object that can be used to 
 * get a request-token; the first step in the oauth flow.
 *
 * @param	string			The URL where we can get the request-token
 * @param	string			The callback to which the request token is sent.
 * @return	Request			Request object for token-request
 */
rc::Request oAuth::getRequestTokenRequest(const string& url, const string& callbackURL) {
	updateNonce();

	rcp::Collection col = getDefaultParameters();
	col["oauth_callback"] = callbackURL; // out of bound (we only focus on pin!)
	
	// generate the signature. (still w/o access token) 
	string signature = rco::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;
	
	// use the collection to generate a oAuth header with the signature
	string header = "Authorization: " +rco::Header::getHeader(col);

	// create the request object which 
	rc::Request req;
	req.setURL(url);
	req.addHeader(header);
	return req;
}


rc::Request oAuth::getAccessTokenRequest(const string& url) {
	// get the parameters necessary for a token-request.
	rcp::Collection col = getDefaultParameters();
	col["oauth_token"] = getTokenKey();
	col["oauth_verifier"] = getPin();
	
	string signature = rco::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;
	
	string header = "Authorization: " +rco::Header::getHeader(col);
	rc::Request req;
	req.setURL(url);
	req.addHeader(header);
	return req;

}

void oAuth::authorize(rc::Request& req) {
	updateNonce();
	string s =getAuthorizationHeader(req, req.getParams());
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
	printf("%s\n", s.c_str());
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
	req.addHeader(getAuthorizationHeader(req, req.getParams()));
}

string oAuth::getAuthorizationHeader(rc::Request& req, const rcp::Collection& params) {
	return "Authorization: " +getHeader(req, params);
}

string oAuth::getHeader(rc::Request& req, const rcp::Collection& params) {
	// create collection for signature
	rcp::Collection signature_params = getDefaultParameters();
	signature_params += params;
	
	signature_params["oauth_token"] = getTokenKey();
	string signature;
	if(req.isGet()) {
		signature = rco::Signature::getSignatureForGet(*this, req.getURL(),signature_params);
	}
	else if(req.isPost()) {
		signature = rco::Signature::getSignatureForPost(*this, req.getURL(),signature_params);
	}

	// Create header with signature	
	rcp::Collection header_params = getDefaultParameters();
	header_params["oauth_signature"] = signature;
	header_params["oauth_token"] = getTokenKey();
	return rco::Header::getHeader(header_params);
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
