#ifndef ROXLU_TWITTER_CURL_REQUESTH
#define ROXLU_TWITTER_CURL_REQUESTH

#include "../parameter/Parameter.h"
#include "../parameter/Collection.h"
#include "Curl.h"
#include <string>
using std::string;


namespace roxlu {
namespace twitter {
namespace curl {

namespace rtp = roxlu::twitter::parameter;
namespace rtc = roxlu::twitter::curl;

class Request {
public:
	Request();
	Request(const string& u);
	~Request();
	
	enum {
		 REQUEST_GET
		,REQUEST_POST
		,REQUEST_DELETE
	};
	
	void setHeader(const string& header);
	string& getHeader();
	void setURL(const string& url);
	string& getURL();
	bool doGet(rtc::Curl& curl,  string& result);
	bool doGet(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams);
	bool doPost(rtc::Curl& curl,  string& result, bool multipart = false);
	bool doPost(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams);
	void addParams(const rtp::Collection& extraParams);
	string getQueryString();
	rtp::Collection& getParams();
		
	bool isPost();
	void isPost(bool flag);
	bool isGet();
	void isGet(bool flag);
	bool isDelete();
	void isDelete(bool flag);

private:
	int method;
	rtp::Collection params;
	string header;
	string url;
};

inline rtp::Collection& Request::getParams() {
	return params;
}

inline string& Request::getURL() {
	return url;
}

inline string& Request::getHeader() {
	return header;
}

}}} // roxlu::twitter::Curl

#endif