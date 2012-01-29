#include "oAuth.h"
#include "Signature.h"
#include "Header.h"

namespace rtp = roxlu::twitter::parameter;
namespace rto = roxlu::twitter::oauth;
namespace rtc = roxlu::twitter::curl;

namespace roxlu {
namespace twitter {
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
 * @return	Request			Request object for token-request
 */
rtc::Request oAuth::getRequestTokenRequest() {
	updateNonce();
	
	// get the parameters necessary for a token-request.
	rtp::Collection col = getDefaultParameters();
	col["oauth_callback"] = "oob"; // out of bound (we only focus on pin!)
	
	// generate the signature. (still w/o access token)
	string url = "http://api.twitter.com/oauth/request_token";
	string signature = rto::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;
	
	// use the collection to generate a oAuth header with the signature
	string header = rto::Header::getHeader(col);
	
	// create the request object which 
	rtc::Request req;
	req.setURL(url);
	req.setHeader(header);
	return req;
}


rtc::Request oAuth::getAccessTokenRequest() {
	string url = "https://api.twitter.com/oauth/access_token";
	
	// get the parameters necessary for a token-request.
	rtp::Collection col = getDefaultParameters();
	col["oauth_token"] = getTokenKey();
	col["oauth_verifier"] = getPin();
	
	string signature = rto::Signature::getSignatureForGet(*this, url,col);
	col["oauth_signature"] = signature;
	
	string header = rto::Header::getHeader(col);
	rtc::Request req;
	req.setURL(url);
	req.setHeader(header);
	return req;

}

rtc::Request oAuth::getAuthorizedPost(const string& url, const rtp::Collection& params) {
	updateNonce();

	// create collection for signature
	rtp::Collection signature_col = getDefaultParameters();
	signature_col += params; // @todo multi part form data (file) should not be part of these params! http://tools.ietf.org/html/rfc5849#section-3.4.1.3.1
	signature_col["oauth_token"] = getTokenKey();
	string signature = rto::Signature::getSignatureForPost(*this, url,signature_col);

	// Create header with signature	
	rtp::Collection header_col = getDefaultParameters();
	header_col["oauth_signature"] = signature;
	header_col["oauth_token"] = getTokenKey();
	string header = rto::Header::getHeader(header_col);
	
	rtc::Request req;
	req.setURL(url);
	req.setHeader(header);
	return req;
}

void oAuth::authorize(rtc::Request& req) {
	updateNonce();
	
	// create signature.
	rtp::Collection signature_params = getDefaultParameters();
	signature_params += req.getParams();
	signature_params["oauth_token"] = getTokenKey();
//	signature_params.print();
	string signature;
	
	if(req.isPost()) {
		signature  = rto::Signature::getSignatureForPost(*this, req.getURL(), signature_params);
	}
	else if(req.isGet()) {
		signature = rto::Signature::getSignatureForGet(*this, req.getURL(), signature_params);
	}
	
	// create header.
	rtp::Collection header_params = getDefaultParameters();
	header_params["oauth_signature"] = signature;
	header_params["oauth_token"] = getTokenKey();
	string header = rto::Header::getHeader(header_params);
	
	req.setHeader(header);
}


rtc::Request oAuth::getAuthorizedGet(const string& url, const rtp::Collection& params) {
	updateNonce();
	
	// create collection for signature
	rtp::Collection signature_col = getDefaultParameters();
	signature_col += params; // @todo multi part form data (file) should not be part of these params! http://tools.ietf.org/html/rfc5849#section-3.4.1.3.1
	signature_col["oauth_token"] = getTokenKey();
	string signature = rto::Signature::getSignatureForGet(*this, url,signature_col);

	// Create header with signature	
	rtp::Collection header_col = getDefaultParameters();
	header_col["oauth_signature"] = signature;
	header_col["oauth_token"] = getTokenKey();
	string header = rto::Header::getHeader(header_col);
	
	rtc::Request req;
	req.setURL(url);
	req.setHeader(header);
	req.addParams(params);
	return req;

}

rtp::Collection oAuth::getDefaultParameters() {
	rtp::Collection col;
	col["oauth_consumer_key"] = getConsumerKey();
	col["oauth_signature_method"] = getSignatureMethod();
	col["oauth_timestamp"] = timestamp;
	col["oauth_nonce"] = nonce;
	col["oauth_version"] = getVersion();
	return col;
}

}}} // roxlu::twitter::oauth
