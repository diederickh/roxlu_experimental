#ifndef ROXLU_TWEETURLH
#define ROXLU_TWEETURLH


#include <fstream>
#include <string>
#include "../../libs/curl/curl.h"

using std::string;

namespace roxlu {

struct TweetURL {
	enum TweetURLType {
		 NONE
		,TWITPIC
	};
	TweetURL() {}
	TweetURL(string url);
	TweetURL& operator=(const TweetURL& other);
	~TweetURL();
	
	void detectURLType();
	bool isTwitPic();
	string getTwitPicURL();
	string getTwitPicCode();
	string getURL();
	void update();
	bool download(string fromURL, string name);
	static size_t writeHeader( void *ptr, size_t size, size_t nmemb, void *userdata);
	static size_t writeData(void *ptr, size_t size, size_t nmemb, void* obj);

	std::ofstream* fs;
	bool isDownloaded();
	
	
private:	
	bool cleanup();
	void createFile();
	string file_name;
	string file_extension; // when downloading i.e. twitpic 	
	bool is_downloaded;
	bool is_initialized;
	string url;
	int type;
	CURL* curl;
	CURLM* curlm;
	
};

inline bool TweetURL::isTwitPic() {
	return type == TWITPIC;
}

inline string TweetURL::getURL() {
	return url;
}

inline bool TweetURL::isDownloaded() {
	return is_downloaded;
}

}; // roxlu

#endif