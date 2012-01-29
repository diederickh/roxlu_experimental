#ifndef ROXLU_TWEETH
#define ROXLU_TWEETH

#include <string>
#include <vector>
#include "URL.h"

using std::string;
using std::vector;

namespace roxlu {
namespace twitter {
namespace type {

namespace rtt = roxlu::twitter::type; 

// @todo implement a age() function which returns the number of seconds
// ago this tweet was created.
class Tweet {
public:
	Tweet();
	~Tweet();
	void setText(string t) 			{ text = t; 			}
	void setScreenName(string n) 	{ screen_name = n; 		}
	void setAvatar(string a) 		{ avatar = a; 			}
	void setUserID(string uid)		{ user_id = uid; 		}
	void setTweetID(string tid) 	{ tweet_id = tid; 		}
	void addURL(rtt::URL* url);
	
	string getText() const			{ return text; 			}		
	string getScreenName() const	{ return screen_name;	}
	string getAvatar() const		{ return avatar;		}
	string getUserID() const 		{ return user_id; 		}
	string getTweetID() const		{ return tweet_id;		}
	vector<rtt::URL*>& getURLs();
	
	string text;
	string tweet_id;
	string user_id;
	string screen_name;
	string avatar;
	vector<rtt::URL*> urls;
};

}}} // roxlu::twitter::type


#endif
