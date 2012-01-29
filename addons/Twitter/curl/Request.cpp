#include "Request.h"

namespace roxlu {
namespace twitter {
namespace curl {

namespace rtc = roxlu::twitter::curl;

Request::Request() 
	:header("")
	,method(REQUEST_GET)
{
	
}

Request::Request(const string& u) 
	:header("")
	,method(REQUEST_GET)
	,url(u)
{
}

Request::~Request() {
	printf("~Request()\n");
}

void Request::setURL(const string& u) {
	url = u;
}

void Request::setHeader(const string& h) {
	header = h;
}

bool Request::doGet(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams) {
	params += extraParams;
	return doGet(curl, result);
}

void Request::addParams(const rtp::Collection& extraParams) {
	params += extraParams;
}

bool Request::doGet(rtc::Curl& curl, string& result) {
	// is the url set?
	if(!url.length()) {
		return false;
	}
	
	if(header.length()) {
		curl.setHeader(header);
	}
	
	// add params to url  // @todo test
	// @todo call curl.createQueryString!
	// @todo ^^--> no, use params.getQueryString(), also need to update curl to do the same 
	// @todo ^^--> no,no use this.getQueryString() ;-)
	// --------------------------------
	//curl.setVerbose(true);
	string ps;
	const list<rtp::Parameter*>& pars = params.getParameters();
	list<rtp::Parameter*>::const_iterator it = pars.begin();
	while(it != pars.end()) {
		ps.append((*it)->getName());
		ps.append("=");
		ps.append((*it)->getStringValue());
		
		++it;
		if(it != pars.end()) {
			ps.append("&");
		}	
	}
	string use_url = url;
	if(ps.length()) {
		use_url.append("?");
		use_url.append(ps);
	}
	// ------------------------
	
	if(!curl.doGet(use_url)) {
		return false;
	}
	
	result = curl.getBuffer();
	return true;
}

string Request::getQueryString() {
	return params.getQueryString();
}

bool Request::doPost(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams) {
	params += extraParams;
	return doPost(curl, result);
}

bool Request::doPost(rtc::Curl& curl,  string& result, bool multipart) {
	if(!url.length()) {
		return false;
	}
	
	if(header.length()) {
		curl.setHeader(header);
	}

	curl.setVerbose(true);

	if(!curl.doPost(url, params, multipart)) {
		return false;
	}
	
	result = curl.getBuffer();
	return true;
}


bool Request::isPost() {
	return method == REQUEST_POST;
}

void Request::isPost(bool flag) {
	method = REQUEST_POST;
}

bool Request::isGet() {
	return method == REQUEST_GET;
}

void Request::isGet(bool flag) {
	method = REQUEST_GET;
}

bool Request::isDelete() {
	return method == REQUEST_DELETE;
}

void Request::isDelete(bool flag) {
	method = REQUEST_DELETE;
}


}}} // roxlu::twitter::curl