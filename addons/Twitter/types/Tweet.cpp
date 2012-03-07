#include "Tweet.h"

namespace roxlu {
namespace twitter {
namespace type {

namespace rtt = roxlu::twitter::type; 

Tweet::Tweet() {
}

Tweet::~Tweet() {
	
	vector<rtt::URL*>::iterator it = urls.begin();
	while(it != urls.end()) {
		delete *it;
		it = urls.erase(it);
	}
		
}

void Tweet::addURL(rtt::URL* url) {
	urls.push_back(url);
}

vector<rtt::URL*>& Tweet::getURLs() {
	return urls;
}

void Tweet::addTag(const string& tag) {
	tags.push_back(tag);
}

void Tweet::addUserMention(const string& screenName) {
	user_mentions.push_back(screenName);
}

void Tweet::setCreatedAt(const string& date) {
	struct tm tm;
	strptime(date.c_str(), "%a %b %d %H:%M:%S %z %Y", &tm);

	created_at_timestamp = mktime(&tm);
	
	char buffer[80];
	strftime(buffer, 80, "%a, %e %b %Y %H:%M:%S %Z", &tm);
	created_at_string = buffer;
}

void Tweet::setCreatedAt(const time_t& timestamp) {
	created_at_timestamp = timestamp;
	created_at_string = ctime(&timestamp);
}



}}} // roxlu::twitter::data