#ifndef ROXLU_TWITTER_STREAMH
#define ROXLU_TWITTER_STREAMH

#include <vector>
#include "../Curl/Request.h"
#include "../Curl/oauth/Header.h"
#include "../Curl/oauth/Signature.h"
#include "../../libs/curl/curl.h"
#include "../../libs/jansson/jansson.h"
#include "Twitter.h"

using std::vector;

// @todo read this: https://dev.twitter.com/docs/streaming-api/user-streams/suggestions


const string URL_STREAM_USER = "https://userstream.twitter.com/2/user.json";
const string URL_STREAM_FILTER = "https://stream.twitter.com/1/statuses/filter.json";

namespace roxlu {
namespace twitter {

class Stream {
public:

	Stream(Twitter& twitter);
	~Stream();
	bool connect(const string& streamURL);	
	bool disconnect();
	bool isConnected();
	bool update();
	
	// Parameters
	void follow(const vector<string>& followers);
	bool getFollowList(string& result);
	void track(const string& tag); // adds a hashtag to the list of hashtags to track.
	bool getTrackList(string& result);
	
	// Callback & helpers
	static size_t curlWriteCallback(char *ptr, size_t size, size_t nmemb, Stream* obj);
	void parseBuffer();
	std::string& trim(std::string &s);
 	std::string& ltrim(std::string &s);
	std::string& rtrim(std::string &s);
	
	string buffer;

private:
	struct curl_slist* curl_header;
	vector<string> to_follow;
	vector<string> to_track;
	CURL* curl;
	CURLM* curlm;
	Twitter& twitter;
	bool connected;
};

inline std::string& Stream::trim(std::string &s) {
	return ltrim(rtrim(s));
}

inline std::string& Stream::ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

inline std::string& Stream::rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

inline bool Stream::isConnected(){
	return connected;
}

}} // roxlu::twitter

#endif