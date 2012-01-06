#ifndef ROXLU_TWEETH
#define ROXLU_TWEETH

#include <string>
#include <vector>
#include "TweetURL.h"

using std::string;
using std::vector;

namespace roxlu {


class Tweet {
public:
	Tweet();
	~Tweet();
	void setText(string t) 			{ text = t; 			}
	void setScreenName(string n) 	{ screen_name = n; 		}
	void setAvatar(string a) 		{ avatar = a; 			}
	void setUserID(string uid)		{ user_id = uid; 		}
	void setTweetID(string tid) 	{ tweet_id = tid; 		}
	void addURL(TweetURL* url);
	
	string getText()			{ return text; 			}		
	string getScreenName() 		{ return screen_name;	}
	string getAvatar() 			{ return avatar;		}
	string getUserID() 			{ return user_id; 		}
	string getTweetID()			{ return tweet_id;		}
	vector<TweetURL*>& getURLs();
	
	string text;
	string tweet_id;
	string user_id;
	string screen_name;
	string avatar;
	vector<TweetURL*> urls;
};

}; // roxlu


#endif
