#include "Request.h"

namespace roxlu {
namespace curl {

namespace rc = roxlu::curl;

Request::Request() 
	:method(REQUEST_GET)
	,is_multipart(false)
{
	
}

Request::Request(const string& u) 
	:method(REQUEST_GET)
	,url(u)
	,is_multipart(false)
{
}

Request::~Request() {
	printf("~Request()\n");
}

void Request::setURL(const string& u) {
	url = u;
}

void Request::addHeader(const string& h) {
	headers.push_back(h);
}

bool Request::doGet(rc::Curl& curl,  string& result, const rcp::Collection& extraParams) {
	params += extraParams;
	return doGet(curl, result);
}

void Request::addParams(const rcp::Collection& extraParams) {
	params += extraParams;
}

bool Request::doGet(rc::Curl& curl, string& result) {
	// is the url set?
	if(!url.length()) {
		return false;
	}
	
	vector<string>::iterator head_it = headers.begin();
	while(head_it != headers.end()) {
		curl.addHeader(*head_it);
		++head_it;
	}
	
	//curl.setVerbose(true);
	
	// create url + query string.	
	string use_url = url;
	use_url += getQueryString();	

	if(!curl.doGet(use_url)) {
		return false;
	}
	
	result = curl.getBuffer();
	return true;
}

string Request::getQueryString() {
	return params.getQueryString();
}

bool Request::doPost(rc::Curl& curl,  string& result, const rcp::Collection& extraParams) {
	params += extraParams;
	return doPost(curl, result);
}

bool Request::doPost(rc::Curl& curl,  string& result, bool multipart) {
	if(!url.length()) {
		return false;
	}
	
	// adding headers.
	vector<string>::iterator head_it = headers.begin();
	while(head_it != headers.end()) {
		curl.addHeader(*head_it);
		++head_it;
	}
	
	//curl.setVerbose(true);

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
	if(flag) {
		isMultiPart(false);
	}
}

bool Request::isDelete() {
	return method == REQUEST_DELETE;
}

void Request::isDelete(bool flag) {
	method = REQUEST_DELETE;
}

void Request::isMultiPart(bool flag) {
	is_multipart = flag;
}

bool Request::isMultiPart() {
	return is_multipart;
}

}} // roxlu::twitter::curl