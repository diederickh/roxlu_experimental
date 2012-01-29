#ifndef ROXLU_TWITTER_CURL_CURLH
#define ROXLU_TWITTER_CURL_CURLH

#define CHECK_CURL_ERROR(result)	if((result) != CURLE_OK) { printf("Curl error: %s.\n", curl_easy_strerror((result))); return false; }
#define CHECK_CURLM_ERROR(result)	if((result) != CURLM_OK) { printf("Curl multi error: %s.\n", curl_multi_strerror((result))); return false; }


#include <string>
#include "../../../libs/curl/curl.h"
#include "../parameter/Collection.h"
#include "../parameter/Parameter.h"
#include "../../../libs/crypto/urlencode.h"

using std::string;

namespace roxlu {
namespace twitter {
namespace curl {

namespace rtp = roxlu::twitter::parameter;

class Curl {
public:
	Curl();
	~Curl();
	bool isInitialized();
	bool doGet(const string& url);
	bool doPost(const string& url, const rtp::Collection& params, bool multiPart = false);
	void setAuthUsername(const string& username);
	void setAuthPassword(const string& password);
	string& getAuthUsername();
	string& getAuthPassword();
	void setHeader(const string& header);
	void setVerbose(bool verbose);
	string createQueryString(const list<rtp::Parameter*>& queryParams); // @todo use Collection::getQueryString
	string& getBuffer();
	string buffer;
private:
	void reset(); // clears buffer; sets callback, sets userspass
	void setCallback();
	void setUserPass();
	static size_t callback(char* data, size_t size, size_t nmemb, Curl* twit);
	

	CURL* curl;
	string header;	
	bool is_initialized;
	bool is_userpass_set;
	bool is_callback_set;
	string auth_username;
	string auth_password;
	
	//CURLM* curlm; // for now we do not yet use the multi handle... maybe later
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

}}} // roxlu::twitter::curl
#endif