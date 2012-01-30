#ifndef ROXLU_TWITTER_TYPES_GENERALH
#define ROXLU_TWITTER_TYPES_GENERALH

// Collection of simple types, mostly used for event handling.
#include <stdint.h>
#include <string>

using std::string;

namespace roxlu {
namespace twitter {
namespace type {

struct StatusDestroy {
	string user_id_str;
	string id_str;
	uint64_t id;
	uint64_t user_id;
};

}}} // roxlu::twitter::type

#endif