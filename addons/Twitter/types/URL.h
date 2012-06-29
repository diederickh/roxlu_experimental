#ifndef ROXLU_TWEETURLH
#define ROXLU_TWEETURLH


#include <fstream>
#include <string>

#if INTPTR_MAX == INT32_MAX
	#include "../../../external/includes/curl32/curl.h"
#elif INTPTR_MAX == INT64_MAX
	#include "../../../external/includes/curl64/curl.h"
#endif





using std::string;

namespace roxlu {
namespace twitter {
namespace type {

struct URL {
	enum TweetURLType {
		 NONE
		,TWITPIC
		,INSTAGRAM
	};
	URL() {}
	URL(string url);
	URL& operator=(const URL& other);
	~URL();
	
	void detectURLType();
	bool isTwitPic();
	bool isInstagram();
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
	bool createFile();
	string file_name;
	string file_extension; // when downloading i.e. twitpic 	
	bool is_downloaded;
	bool is_initialized;
	string url;
	int type;
	CURL* curl;
	CURLM* curlm;
	
};

inline bool URL::isTwitPic() {
	return type == TWITPIC;
}

inline bool URL::isInstagram() {
	return type == INSTAGRAM;
}

inline string URL::getURL() {
	return url;
}

inline bool URL::isDownloaded() {
	return is_downloaded;
}

}}} // roxlu::twitter::type

#endif