#ifndef ROXLU_TWITTER_CURL_CURLH
#define ROXLU_TWITTER_CURL_CURLH

#define CHECK_CURL_ERROR(result)	if((result) != CURLE_OK) { printf("Curl error: %s.\n", curl_easy_strerror((result))); return false; }
#define CHECK_CURLM_ERROR(result)	if((result) != CURLM_OK) { printf("Curl multi error: %s.\n", curl_multi_strerror((result))); return false; }

#include <map>
#include <vector>
#include <string>
#include "parameter/Collection.h"
#include "parameter/Parameter.h"
#include "../../libs/curl/curl.h"
#include "../../libs/crypto/urlencode.h"

/**
 * @todo	We need to add some more info to the Parameter types i.e. 
 * 			does the caller wants it to be a post, get or url variable?
 *			Maybe we need to add a "usage" member to Parameter and 
 *			usage values like: USAGE_QUERY_STRING, USAGE_POST
 *
 *
 */

using std::string;
using std::vector;
using std::map;

namespace roxlu {
namespace curl {

namespace rcp = roxlu::curl::parameter;

class Curl {
public:
	Curl();
	~Curl();
	bool isInitialized();
	bool doGet(const string& url);
	bool doPost(const string& url, const rcp::Collection& params, bool multiPart = false);
	void setAuthUsername(const string& username);
	void setAuthPassword(const string& password);
	string& getAuthUsername();
	string& getAuthPassword();
	void addHeader(const string& header);
	void setVerbose(bool verbose);
	void addResponseHeader(const string& name, const string& value);
	bool getResponseHeader(const string& name, string& result);
	string getHTTPResponseMessage();
	long getHTTPResponseCode();
	string createQueryString(const list<rcp::Parameter*>& queryParams); // @todo use Collection::getQueryString
	string& getBuffer();
	string buffer;
	
	
private:
	void reset(); // clears buffer; sets callback, sets userspass
	void setDefaultOptions();
	void setCallback();
	void setUserPass();
	void setHeaders(struct curl_slist** slist);
	static size_t writeCallback(char* data, size_t size, size_t nmemb, Curl* c);
	static size_t headerCallback(char* data, size_t size, size_t nmemb, Curl* c);

	CURL* curl;
	string header;	
	bool is_initialized;
	bool is_userpass_set;
	bool is_callback_set;
	string auth_username;
	string auth_password;
	vector<string> headers;
	map<string, string> response_headers;
	long http_response_code;
	string http_response_message;
};

inline string& Curl::getBuffer() {
	return buffer;
}

inline bool Curl::isInitialized() {
	return is_initialized;
}

inline void Curl::setAuthUsername(const string& username) {
	auth_username = username;
}

inline void Curl::setAuthPassword(const string& password) {
	auth_password = password;
}

inline string& Curl::getAuthUsername() {
	return auth_username;
}

inline string& Curl::getAuthPassword() {
	return auth_password;
}

inline void Curl::addResponseHeader(const string& name, const string& value) {
	response_headers.insert(std::pair<string, string>(name, value));
}

inline bool Curl::getResponseHeader(const string& name, string& result) {	
	map<string, string>::iterator it = response_headers.find(name);
	if(it == response_headers.end()) {
		return false;
	}
	result = it->second;
	return true;
}

inline string Curl::getHTTPResponseMessage() {
	return http_response_message;
}

inline long Curl::getHTTPResponseCode() {
	return http_response_code;
}

}} // roxlu::curl
#endif