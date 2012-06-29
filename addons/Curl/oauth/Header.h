#ifndef ROXLU_TWITTER_OAUTH_HEADERH
#define ROXLU_TWITTER_OAUTH_HEADERH

#include <string>
#include "../parameter/Collection.h"
#include "../parameter/Parameter.h"

namespace rcp = roxlu::curl::parameter;

namespace roxlu {
namespace curl {
namespace oauth {

class Header {
public:
	static std::string getHeader(const rcp::Collection& col);
};

}}} // roxlu::curl::oauth

#endif