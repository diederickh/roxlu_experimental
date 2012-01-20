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
	~Request();
	
	void setHeader(const string& header);
	void setURL(const string& url);
	bool doGet(rtc::Curl& curl,  string& result);
	bool doGet(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams);
	bool doPost(rtc::Curl& curl,  string& result);
	bool doPost(rtc::Curl& curl,  string& result, const rtp::Collection& extraParams);
	void addParams(const rtp::Collection& extraParams);
	rtp::Collection& getParams();
	
private:
	rtp::Collection params;
	string header;
	string url;
};

inline rtp::Collection& Request::getParams() {
	return params;
}

}}} // roxlu::twitter::Curl

#endif