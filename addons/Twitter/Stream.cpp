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
	,reconnect_on(0)
	,reconnect_delay(10) // 10 seconds
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
	twitter.accountVerifyCredentials();
	printf("Credentials: %s\n", twitter.getResponse().c_str());
	
	CURLcode r;
	
	// create request.
	string url = streamURL;
	connected_url = streamURL;
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

	// parse headers.
	r = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &Stream::curlHeaderCallback);
	CHECK_CURL_ERROR(r);	
	
	// object gets passed to header function
	r = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, this);
	CHECK_CURL_ERROR(r);	
	
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, true);

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
	CURLMcode cm = curl_multi_add_handle(curlm, curl);
	CHECK_CURLM_ERROR(cm);	

	connected = true;
	
	// let all listeners know we got connected.
	vector<IStreamEventListener*>::iterator it = event_listeners.begin();
	while(it != event_listeners.end()) {
		(*it)->onTwitterStreamConnected();
		++it;
	}
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
		if(reconnect_on == 0) {
			time_t seconds;
			seconds = time(NULL);
			printf("Seconds: %ld\n", seconds);
			reconnect_on = seconds + reconnect_delay;
			reconnect_delay *= reconnect_delay;
			printf("Next time we disconnect we start after: %d seconds\n", reconnect_delay);
			
			// let all listeners know we got disconnected.
			vector<IStreamEventListener*>::iterator it = event_listeners.begin();
			while(it != event_listeners.end()) {
				(*it)->onTwitterStreamDisconnected();
				++it;
			}
		}
		else {
			time_t now = time(NULL);
			if(now > reconnect_on) {
				printf("Twitter is reconnecting\n");
				reconnect_delay = 10; // reset.
				reconnect_on = 0;
				disconnect();
				connect(connected_url);
			}
		}
//		printf("Twitter stream not running...\n");
		return false;
	}
	else {
				
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

void Stream::clearTrackList() {
	to_track.clear();
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
void Stream::addResponseHeader(const string& name, const string& value) {
	response_headers.insert(std::pair<string, string>(name, value));
}

bool Stream::getResponseHeader(const string& name, string& result) {
	result.clear();
	map<string, string>::iterator it = response_headers.find(name);
	if(it == response_headers.end()) {
		return false;
	}
	result = it->second;
	return true;
}


size_t Stream::curlWriteCallback(char *ptr, size_t size, size_t nmemb, Stream* obj) {
	size_t bytes_to_write = size * nmemb;
	obj->buffer.append(ptr, bytes_to_write);
//	for(int i = 0; i < bytes_to_write; ++i) {
//		printf("%c", ptr[i]);
//	}
	obj->parseBuffer();
	return bytes_to_write;
}

size_t Stream::curlHeaderCallback(char* ptr, size_t size, size_t nmemb, Stream* obj) {
	size_t bytes_to_write = size * nmemb;

	// store the headers
	string header_line(ptr, nmemb);
	size_t pos = header_line.find_first_of(":");
	if(pos != string::npos) {
		string header_name = header_line.substr(0,pos);
		string header_value = header_line.substr(pos+2, (header_line.length()-(pos+4)));
		std::transform(header_name.begin(), header_name.end(), header_name.begin(),::tolower);
		obj->addResponseHeader(header_name, header_value);
	}
	else {
		// parse the HTTP result.
		pos = header_line.find("HTTP/");
		if(pos != std::string::npos) {
			std::istringstream oss(header_line);
			long code;
			string msg;
			oss >> msg >> code >> msg;;
			if(code == 401) {
				printf("Error: while connecting to twitter server: %s, code: %lu\n", msg.c_str(), code);
//				exit(0);
			}
		}
	}
	return bytes_to_write;
}

}} // roxlu::twitter