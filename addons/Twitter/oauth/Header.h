#ifndef ROXLU_TWITTER_OAUTH_HEADERH
#define ROXLU_TWITTER_OAUTH_HEADERH

#include <string>
#include "../parameter/Collection.h"
#include "../parameter/Parameter.h"

namespace rtp = roxlu::twitter::parameter;

namespace roxlu {
namespace twitter {
namespace oauth {

class Header {
public:
	static std::string getHeader(const rtp::Collection& col);
};

}}} // roxlu::twitter::oauth

#endif