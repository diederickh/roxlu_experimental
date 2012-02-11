#include "Stream.h"
#include <sstream>

namespace rcp = roxlu::curl::parameter;
namespace rc = roxlu::curl;

namespace roxlu {
namespace twitter {

Stream::Stream(Twitter& tw) 
	:twitter(tw)
	,connected(false)
	,curl_header(NULL)
{
	
}

Stream::~Stream() {
	disconnect();
}


/*
@todo 	pass param to which stream one once to connect (general, user, site)
@todo	implement reconnecting to server when hashtags to track changes
*/
bool Stream::connect(const string& streamURL) {
	CURLcode r;
	
	// create request.
	string url = streamURL;
	rcp::Collection params;
	rc::Request req;
	req.setURL(url);
	req.isGet(true);
	

	// TRACK : https://dev.twitter.com/docs/streaming-api/methods#statuses-filter
	// ------
	string track_list;
	if(getTrackList(track_list)) {
		req.getParams().addString("track",track_list);
		printf("Tracklist: %s\n", track_list.c_str());
	}
	
	// FOLLOW
	// ------
	string follow_list;
	if(getFollowList(follow_list)) {
		req.getParams().addString("follow", follow_list);
	}
	
	string qs = req.getQueryString();
	string use_url = url;
	if(qs.length()) {
		use_url.append(qs);
	}
	twitter.getoAuth().authorize(req);

	printf("Using url: %s\n", use_url.c_str());
	
	// create curl handle.
	curl = curl_easy_init();
	if(!curl) {	
		curl_easy_cleanup(curl);
		printf("Error: cannot create easy handle.\n");
		return false;
	}
	
	/*
	string userpass = twitter.getTwitterUsername() +":" +twitter.getTwitterPassword();
	curl_easy_setopt(curl, CURLOPT_USERPWD, NULL); 
	curl_easy_setopt(curl, CURLOPT_USERPWD, userpass.c_str());
	*/
	
	// set url
	r = curl_easy_setopt(curl, CURLOPT_URL, use_url.c_str());
	CHECK_CURL_ERROR(r);
	
	// no CA verification.
	r = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	CHECK_CURL_ERROR(r);

	// called when data is received (unencrypted)
	r = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Stream::curlWriteCallback);
	CHECK_CURL_ERROR(r);	
	
	// object we get passed into writeData
	r = curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	CHECK_CURL_ERROR(r);	
	
	curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

	// set the oauth headers.
	const vector<string>& headers = req.getHeaders();
	if(headers.size()) {
		vector<string>::const_iterator it = headers.begin();
		while(it != headers.end()) {
			curl_header = curl_slist_append(curl_header, (*it).c_str());
			if(curl_header == NULL) {
				printf("Error: stream cannot set header!\n");
				return false;
			}
			++it;
		}
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER ,curl_header);
	}	
	
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

bool Stream::disconnect() {
	if(connected) {
		CURLcode cc;
		CURLMcode cmc;
		
		connected = false;

		// clean up header.
		if(curl_header != NULL) {
			curl_slist_free_all(curl_header);
		}
		cc = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL); 
		CHECK_CURL_ERROR(cc);

		// remove handles from multi collection.		
		cmc = curl_multi_remove_handle(curlm, curl);
		CHECK_CURLM_ERROR(cmc);	
		
		// cleanup handles.
		curl_easy_cleanup(curl);
		cmc = curl_multi_cleanup(curlm);
		CHECK_CURLM_ERROR(cmc);	
		
		curl = NULL;
		curlm = NULL;
		curl_header = NULL;
	}
	return true;
}


bool Stream::update() {
	int still_running = 0;
	CURLMcode r = curl_multi_perform(curlm, &still_running);
	CHECK_CURLM_ERROR(r);
	
	if(still_running == 0) {
		printf("Twitter stream not running...\n");
		return false;
	}
	return true;
}

// vector with user screen names to follow
void Stream::follow(const vector<string>& followers) {
	std::copy(followers.begin(), followers.end(), std::back_inserter(to_follow));
}

bool Stream::getFollowList(string& result) {
	result.clear();
	vector<string>::iterator it = to_follow.begin();
	while(it != to_follow.end()) {
		result.append(*it);
		++it;
		if(it != to_follow.end()) {
			result.append(",");		
		}
	}
	return result.length();
}

/**
 * Adds a keyword to track. Use this before connecting 
 * to the stream.
 *
 * @param	string	hashTag		A keyword to track, w/o the # character.
 * @return	void
 */
void Stream::track(const string& hashTag) {
	to_track.push_back(hashTag);
}

/**
 * Get a list with keywords to track. 
 *
 * @param	string[out]		Is set to the list of tracks.
 */
bool Stream::getTrackList(string& result) {
	result.assign("");
	vector<string>::iterator it = to_track.begin();
	while(it != to_track.end()) {
		result.append(*it);
		++it;
		if(it != to_track.end()) {
			result.append(",");
		}	
	}
	return result.length();	
}

void Stream::parseBuffer() {
	// after we received the http response
	std::stringstream ss(buffer);
	string line;
	int num_chars = 0;
	while(std::getline(ss, line, '\r')) {
		int n = line.size();	
		if(n == 1) {
			num_chars += 1;
			continue;
		}
		num_chars += n + 1;		
		line = Stream::trim(line);
		twitter.getJSON().parse(line);
	}
	
	if(num_chars > 0) {
		buffer.erase(0, num_chars);
	}
}

size_t Stream::curlWriteCallback(char *ptr, size_t size, size_t nmemb, Stream* obj) {
	size_t bytes_to_write = size * nmemb;
	obj->buffer.append(ptr, bytes_to_write);
	for(int i = 0; i < bytes_to_write; ++i) {
		printf("%c", ptr[i]);
	}
	obj->parseBuffer();
	return bytes_to_write;
}

}} // roxlu::twitter