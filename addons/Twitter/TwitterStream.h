#ifndef ROXLU_TWITTERSTREAMH
#define ROXLU_TWITTERSTREAMH

#include <string>
#include <deque>
#include <vector>
#include "../../libs/curl/curl.h"
#include "../../libs/jansson/jansson.h"
#include "types/Tweet.h"
#include "types/URL.h"

using std::string;
using std::vector;
using std::deque;

namespace rtt = roxlu::twitter::type;

namespace roxlu {

typedef void (*image_downloaded_callback)(rtt::URL*, void* userdata);
typedef void (*tweet_callback)(rtt::Tweet*, void* userData);

class TwitterStream {
public:
	TwitterStream();
	~TwitterStream();
	bool connect(string username, string password, const vector<string>& tags);
	bool disconnect();
	bool update();
	static size_t writeData(void *ptr, size_t size, size_t nmemb, void* obj);
	void parseBuffer();
	void parseTweetJSON(string& json);
	bool hasNewTweets();
	rtt::Tweet* getNextTweet();
	void removeTweet(rtt::Tweet* tweet);
	void downloadTwitPic(string fileName, rtt::URL* url);
	
	void setImageDownloadCallback(image_downloaded_callback callback, void* userData);
	void setTweetCallback(tweet_callback callback, void* userData);
	
	// trim
	// ------------------------------------
	static std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
	}
	static std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
	}
	static std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
	}
	
	string buffer;
	
private:
	void* tweet_userdata;
	tweet_callback tweet_cb;
	void* image_download_userdata;
	image_downloaded_callback image_download_cb; 
	deque<rtt::URL*> download_twitpics;
	deque<rtt::Tweet*> tweets;
	bool connected;
	CURL* curl;
	CURLM* curlm;
};

inline bool TwitterStream::hasNewTweets() {
	return tweets.size() > 0;
}

inline rtt::Tweet* TwitterStream::getNextTweet() {
	return tweets.front();
}

inline void TwitterStream::setImageDownloadCallback(image_downloaded_callback callback, void* userData) {
	image_download_cb = callback;
	image_download_userdata = userData;
}

inline void TwitterStream::setTweetCallback(tweet_callback callback, void* userData) {
	tweet_userdata = userData;
	tweet_cb = callback;
}

}; // roxlu

#endif
