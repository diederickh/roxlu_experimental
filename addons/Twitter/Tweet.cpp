#include "Tweet.h"

namespace roxlu {

Tweet::Tweet() {
}

Tweet::~Tweet() {
	vector<TweetURL*>::iterator it = urls.begin();
	while(it != urls.end()) {
		delete *it;
		it = urls.erase(it);
	}
		
}

void Tweet::addURL(TweetURL* url) {
	urls.push_back(url);
}

vector<TweetURL*>& Tweet::getURLs() {
	return urls;
}


};