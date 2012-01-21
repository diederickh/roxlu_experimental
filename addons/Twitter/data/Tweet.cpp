#include "Tweet.h"

namespace roxlu {
namespace twitter {
namespace data {

namespace rtd = roxlu::twitter::data; 

Tweet::Tweet() {
}

Tweet::~Tweet() {
	
	vector<rtd::URL*>::iterator it = urls.begin();
	while(it != urls.end()) {
		delete *it;
		it = urls.erase(it);
	}
		
}

void Tweet::addURL(rtd::URL* url) {
	urls.push_back(url);
}

vector<rtd::URL*>& Tweet::getURLs() {
	return urls;
}


}}} // roxlu::twitter::data