#ifndef ROXLU_TWEETH
#define ROXLU_TWEETH

#include <string>
#include <vector>
#include "URL.h"

using std::string;
using std::vector;

namespace roxlu {
namespace twitter {
namespace data {

namespace rtd = roxlu::twitter::data; 

class Tweet {
public:
	Tweet();
	~Tweet();
	void setText(string t) 			{ text = t; 			}
	void setScreenName(string n) 	{ screen_name = n; 		}
	void setAvatar(string a) 		{ avatar = a; 			}
	void setUserID(string uid)		{ user_id = uid; 		}
	void setTweetID(string tid) 	{ tweet_id = tid; 		}
	void addURL(rtd::URL* url);
	
	string getText()				{ return text; 			}		
	string getScreenName() 			{ return screen_name;	}
	string getAvatar() 				{ return avatar;		}
	string getUserID() 				{ return user_id; 		}
	string getTweetID()				{ return tweet_id;		}
	vector<rtd::URL*>& getURLs();
	
	string text;
	string tweet_id;
	string user_id;
	string screen_name;
	string avatar;
	vector<rtd::URL*> urls;
};

}}} // roxlu::twitter::data


#endif
