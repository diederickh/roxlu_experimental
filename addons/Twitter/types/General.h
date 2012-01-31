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

// source for general event.
struct User {
	string name;
	string id_str;
	string screen_name;
	string location;
	uint64_t statuses_count;
	uint64_t id;
};

// target of general event.
struct TargetObject {
	string name;
	string mode;
	string description;
	User user;	
};

// general event: https://dev.twitter.com/docs/streaming-api/user-streams
struct StreamEvent {
	User target;					// who is affected or owns the affected object
	TargetObject target_object;		// the "changed" object; i.e. a list
	User source; 					// who initiated the event
	string event;					// what kind of event.
};

}}} // roxlu::twitter::type

#endif