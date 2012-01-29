#ifndef ROXLU_TWITTER_IEVENTLISTENERH
#define ROXLU_TWITTER_IEVENTLISTENERH

#include "types/Tweet.h"

namespace rtt = roxlu::twitter::type;

namespace roxlu {
namespace twitter {

// interface for event listeners. 
class IEventListener {
public:
	virtual void onStatusUpdate(const rtt::Tweet& tweet) = 0;
//	virtual void onTweetDelete(const rtt::DeletedTweet& deletedTweet) = 0;
};

}} // roxlu::twitter

#endif
