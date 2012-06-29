#ifndef ROXLU_TWITTER_CURL_REQUESTH
#define ROXLU_TWITTER_CURL_REQUESTH

#include "parameter/Parameter.h"
#include "parameter/Collection.h"
#include "Curl.h"
#include <string>
#include <vector>

using std::vector;
using std::string;


namespace roxlu {
namespace curl {

namespace rcp = roxlu::curl::parameter;
namespace rc = roxlu::curl;

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
	
	void addHeader(const string& header);
	const vector<string>& getHeaders();
	void setURL(const string& url);
	string& getURL();
	bool doGet(rc::Curl& curl,  string& result);
	bool doGet(rc::Curl& curl,  string& result, const rcp::Collection& extraParams);
	bool doPost(rc::Curl& curl,  string& result, bool multipart = false);
	bool doPost(rc::Curl& curl,  string& result, const rcp::Collection& extraParams);
	void addParams(const rcp::Collection& extraParams);
	string getQueryString();
	rcp::Collection& getParams();
		
	bool isPost();
	void isPost(bool flag);
	bool isGet();
	void isGet(bool flag);
	bool isDelete();
	void isDelete(bool flag);
	void isMultiPart(bool flag);
	bool isMultiPart();

private:
	int is_multipart;
	vector<string> headers;
	int method;
	rcp::Collection params;
	//string header;
	string url;
};

inline rcp::Collection& Request::getParams() {
	return params;
}

inline string& Request::getURL() {
	return url;
}

inline const vector<string>& Request::getHeaders() {
	return headers;
}

}} // roxlu::curl

#endif