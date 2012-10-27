#ifndef ROXLU_TWITTER_IEVENTLISTENERH
#define ROXLU_TWITTER_IEVENTLISTENERH

#include "types/Tweet.h"
#include "types/General.h"

namespace rtt = roxlu::twitter::type;

namespace roxlu {
namespace twitter {

// interface for event listeners. 
class IEventListener {
public:
	virtual void onStatusUpdate(const rtt::Tweet& tweet) = 0;
	virtual void onStatusDestroy(const rtt::StatusDestroy& destroy) = 0;
	virtual void onStreamEvent(const rtt::StreamEvent& event) = 0;
};

}} // roxlu::twitter

#endif
