#ifndef ROXLU_TWEETEVENTSH
#define ROXLU_TWEETEVENTSH

#include "TweetURL.h"

namespace roxlu {

class TweetDownloadEvent {
public:
	TweetDownloadEvent(TweetURL* tweetURL)
		:tweet_url(tweetURL)
	{
	}
	
	TweetURL* getTweetURL() {
		return tweet_url;
	}
	TweetURL* tweet_url;
};

}; // roxlu
#endif
