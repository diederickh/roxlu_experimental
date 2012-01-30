#include "TwitterStream.h"
#include "TweetConstants.h"
#include "types/URL.h"
#include "types/Tweet.h"
#include <sstream>
#include <iostream>


using std::stringstream;

namespace rtt = roxlu::twitter::type;

namespace roxlu {

TwitterStream::TwitterStream()
	:connected(false)
	,image_download_cb(NULL)
	,image_download_userdata(NULL)
	,tweet_cb(NULL)
	,tweet_userdata(NULL)
{
}

TwitterStream::~TwitterStream() {
	// Remote tweet urs which are queued to download
	{
		deque<rtt::URL*>::iterator it = download_twitpics.begin();
		while(it != download_twitpics.end()) {
			delete *it;
			it = download_twitpics.erase(it);
		}
	}
	
	// Remove tweets
	{
		deque<rtt::Tweet*>::iterator it = tweets.begin();
		while(it != tweets.end()) {
			delete *it;
			it = tweets.erase(it);
		}
	}
}

bool TwitterStream::connect(string username, string password, const vector<string>& tags) {
	CURLcode r;
	
	// create curl handle.
	curl = curl_easy_init();
	if(!curl) {	
		curl_easy_cleanup(curl);
		printf("Error: cannot create easy handle.\n");
		return false;
	}
	
	// create the url
	stringstream ss;
	ss << "https://stream.twitter.com/1/statuses/filter.json?track=";
	for(int i = 0; i < tags.size(); ++i) {
		ss << tags[i];
		if(i < tags.size()-1) {
			ss << ",";
		}
	}
	ss << "&include_entities=true";

	r = curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
	CHECK_CURL_ERROR(r);
	
	// set login
	stringstream userpass;
	userpass << username << ":" << password;
	r = curl_easy_setopt(curl, CURLOPT_USERPWD, userpass.str().c_str());
	CHECK_CURL_ERROR(r);

	// no CA verification.
	r = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	CHECK_CURL_ERROR(r);

	// called when data is received (unencrypted)
	r = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &TwitterStream::writeData);
	CHECK_CURL_ERROR(r);	
	
	// object we get passed into writeData
	r = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	CHECK_CURL_ERROR(r);	
	
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
	
	// create multi handle for async io
	curlm = curl_multi_init();
	if(curlm == NULL) {
		printf("Cannot initialize curl multi.\n");
		return false;
	}
	
	// add the easy handle to the multi stack.
	CURLMcode t = curl_multi_add_handle(curlm, curl);
	CHECK_CURLM_ERROR(t);	

	connected = true;
	return true;
}

bool TwitterStream::disconnect() {
	if(connected) {
		connected = false;
		CURLMcode r = curl_multi_remove_handle(curlm, curl);
		CHECK_CURLM_ERROR(r);	
		curl_easy_cleanup(curl);
		r = curl_multi_cleanup(curlm);
		CHECK_CURLM_ERROR(r);	
	}
	return true;
}

bool TwitterStream::update() {
	// download twitpics.
	deque<rtt::URL*>::iterator it = download_twitpics.begin();
	while(it != download_twitpics.end()) {
		rtt::URL* u = (*it);
		if(u->isDownloaded()) {
			if(image_download_cb != NULL) {
				image_download_cb(u, image_download_userdata);
			}
			it = download_twitpics.erase(it);
			
		}
		else {
			u->update();
			++it;
		}
	}
		
	// update the buffer...
	if(!connected) {
		return false;
	}
	
	int still_running = 0;
	CURLMcode r = curl_multi_perform(curlm, &still_running);
	CHECK_CURLM_ERROR(r);
	
	if(still_running == 0) {
		printf("Twitter stream not running...\n");
	}
	return true;
}

void TwitterStream::parseBuffer() {
	// after we received the http response
	stringstream ss(buffer);
	string line;
	int num_chars = 0;
	while(std::getline(ss, line, '\r')) {
		int n = line.size();	
		if(n == 1) {
			num_chars += 1;
			continue;
		}
		num_chars += n + 1;		
		line = TwitterStream::trim(line);
		parseTweetJSON(line);
	}
	
	if(num_chars > 0) {
		buffer.erase(0, num_chars);
	}
}

void TwitterStream::parseTweetJSON(string& json) {
	json_t* root;
	json_error_t error;
	rtt::Tweet* tweet;
	
	root = json_loads(json.c_str(), 0, &error);
	if(!root) {
		printf("Error: on line: %d, %s\n", error.line, error.text);
		json_decref(root);
		return;
	}

	tweet = new rtt::Tweet();

	// text
	json_t* node =	json_object_get(root, "text");
	if(!json_is_string(node)) {
		printf("Error: cannot get text from tweet.\n");
		delete tweet;
		json_decref(root);
		return;
	}
	
	string text = json_string_value(node);
	tweet->setText(text);
	
	// id_str
	node = json_object_get(root, "id_str");
	if(!json_is_string(node)) {
		printf("Error: cannot get id_str from tweet.\n");
		delete tweet;
		json_decref(root);
		return;
	}	
	string id = json_string_value(node);
	tweet->setTweetID(id);

	// user - object
	json_t* user = json_object_get(root, "user");
	if(!json_is_object(user)) {
		printf("Error: cannot get user node from tweet.\n");
		delete tweet;
		json_decref(root);
		return;
	}

	// user: screen name
	node = json_object_get(user, "screen_name");
	if(!json_is_string(node)) {
		printf("Error: cannot get user screen_name.\n");
		delete tweet;
		json_decref(root);
		return;
	}
	string screen_name = json_string_value(node);
	tweet->setScreenName(screen_name);
	
	// user: avatar
	node = json_object_get(user, "profile_image_url");
	if(!json_is_string(node)) {
		printf("Error: cannot get profile_image_url.\n");
		delete tweet;
		json_decref(root);
		return;
	}
	string profile_image = json_string_value(node);
	tweet->setAvatar(profile_image);

	// user: id
	node = json_object_get(user, "id_str");
	if(!json_is_string(node)) {
		printf("Error: cannot get user id.\n");
		delete tweet;
		json_decref(root);
		return;
	}
	string user_id = json_string_value(node);
	tweet->setUserID(user_id);
	
	// entities
	json_t* entities = json_object_get(root, "entities");
	if(json_is_object(entities)) {
		json_t* urls = json_object_get(entities, "urls");
		if(json_is_array(urls)) {
			size_t num_urls = json_array_size(urls);
			for(int i = 0; i < num_urls; ++i) {
				json_t* url = json_array_get(urls, i);
				if(url != NULL) {
					json_t* exp_url = json_object_get(url, "expanded_url");
					if(json_is_string(exp_url)) {
						string exp_url_str = json_string_value(exp_url);
						rtt::URL* tu = new rtt::URL(exp_url_str);
						tweet->addURL(tu);
					}
				}
			}
		}
	}

	// append tweet
	tweets.push_back(tweet);
	json_decref(root);
	
	// call 
	if(tweet_cb != NULL) {
		tweet_cb(tweet, tweet_userdata);
	}
}


size_t TwitterStream::writeData(void *ptr, size_t s, size_t nmemb, void* obj) {
	TwitterStream* stream = static_cast<TwitterStream*>(obj);
	size_t bytes_to_write = s * nmemb;
	char* data_ptr = static_cast<char*>(ptr);
	stream->buffer.append(data_ptr, bytes_to_write);
	stream->parseBuffer();
	return bytes_to_write;
}

void TwitterStream::removeTweet(rtt::Tweet* tweet) {
	deque<rtt::Tweet*>::iterator it = std::find(tweets.begin(), tweets.end(), tweet);
	if(it != tweets.end()) {
		tweets.erase(it);
		delete (*it);
	}
}

void TwitterStream::downloadTwitPic(string fileName, rtt::URL* url) {
	if(url->isTwitPic()) {
		download_twitpics.push_back(url);
		url->download(url->getTwitPicURL(), fileName);
	}
}

}; // roxlu