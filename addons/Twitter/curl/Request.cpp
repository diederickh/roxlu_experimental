#include "Request.h"

namespace roxlu {
namespace twitter {
namespace curl {

namespace rtc = roxlu::twitter::curl;

Request::Request() 
	:header("")
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
	// --------------------------------
	curl.setVerbose(true);
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
		printf("USE URL: %s\n", use_url.c_str());
	}
	// ------------------------
	
	if(!curl.doGet(use_url)) {
		return false;
	}
	
	result = curl.getBuffer();
	printf("------------------------------------------------------------\n");
	return true;
}

bool Request::doPost(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams) {
	params += extraParams;
	//params.print();
	return doPost(curl, result);
}



bool Request::doPost(rtc::Curl& curl,  string& result) {
	curl.setVerbose(true);
	if(!url.length()) {
		return false;
	}
	if(header.length()) {
		curl.setHeader(header);
	}

	if(!curl.doPost(url, params)) {
		return false;
	}
	
	result = curl.getBuffer();
	printf("------------------------------------------------------------\n");
	return true;
}


}}} // roxlu::twitter::curl