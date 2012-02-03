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

}}} // roxlu::twitter::data