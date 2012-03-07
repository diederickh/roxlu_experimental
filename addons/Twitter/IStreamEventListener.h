#ifndef ROXLU_TWITTER_STREAM_EVENTLISTENERH
#define ROXLU_TWITTER_STREAM_EVENTLISTENERH

namespace roxlu {
namespace twitter {

class IStreamEventListener {
public:
	virtual void onTwitterStreamDisconnected() = 0;
	virtual void onTwitterStreamConnected() = 0;
};

}} // roxlu::twitter

#endif