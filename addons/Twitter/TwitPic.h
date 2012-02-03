#ifndef ROXLU_TWITTER_TWITPICH
#define ROXLU_TWITTER_TWITPICH

#include <string>
#include "Twitter.h"
#include "Curl/Curl.h"
#include "../../libs/jansson/jansson.h"

using std::string;

namespace rt = roxlu::twitter;

namespace roxlu  {
namespace twitter {

struct TwitPicResult {
	TwitPicResult()
		:width(0)
		,height(0)
		,size(0)
	{
	}
	
	string id_str;
	string text;
	string url;
	int width;
	int height;
	int size;
	string type;
	string timestamp;
	uint64_t user_id;
	string user_screen_name;
	
	void print() {
		printf("id_str: %s\n", id_str.c_str());
		printf("text: %s\n", text.c_str());
		printf("url: %s\n", url.c_str());
		printf("width: %d\n", width);
		printf("height: %d\n", height);
		printf("size: %d\n", size);
		printf("type: %s\n", type.c_str());
		printf("timestamp: %s\n", timestamp.c_str());
		printf("user_id: %llu\n", user_id);
		printf("user_screen_name: %s\n", user_screen_name.c_str());
	}
};

class TwitPic {
public:
	TwitPic(const string& key, rt::Twitter& twitter);
	~TwitPic();
	TwitPic(const TwitPic& other);
	TwitPic& operator=(const TwitPic& other);
	bool upload(const string& filePath, TwitPicResult& result, const string& message = "");
	bool parseJSON(const string& json, TwitPicResult& result);
private:
	
	rt::Twitter& twitter;
	string key;
};

}} // roxlu::twitter

#endif