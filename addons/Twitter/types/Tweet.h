#ifndef ROXLU_TWEETH
#define ROXLU_TWEETH

#include <string>
#include <vector>
#include "URL.h"
#include <sstream>
#include <time.h>


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
	void setCreatedAt(const string& date);
	void setCreatedAt(const time_t& timestamp);
	void setTweetID(string tid) 	{ 
		tweet_id = tid; 		
		
		// get the ID as uint64
		std::stringstream ss;
		ss << tweet_id;
		ss >> tweet_id_int;
	}
	
	void addURL(rtt::URL* url);
	void addTag(const string& tag);
	void addUserMention(const string& screenName);

	
	string getText() const			{ return text; 			}		
	string getScreenName() const	{ return screen_name;	}
	string getAvatar() const		{ return avatar;		}
	string getUserID() const 		{ return user_id; 		}
	string getTweetID() const		{ return tweet_id;		}
	vector<rtt::URL*>& getURLs();
	
	uint64_t tweet_id_int;
	string text;
	string tweet_id;
	string user_id;
	string screen_name;
	string avatar;
	vector<rtt::URL*> urls;
	vector<string> tags; 
	vector<string> user_mentions;
	time_t created_at_timestamp;
	string created_at_string;
};

}}} // roxlu::twitter::type


#endif
