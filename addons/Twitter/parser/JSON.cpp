#include "JSON.h"
#include "../Twitter.h"
#include "../../../libs/curl/curl.h"
#include "../../../libs/jansson/jansson.h"
#include "../types/Tweet.h"

namespace rtt = roxlu::twitter::type;

namespace roxlu {
namespace twitter {
namespace parser {


JSON::JSON(roxlu::twitter::Twitter& tw) 
	:twitter(tw)
{
}

JSON::~JSON() {
	printf("~JSON()");
}

bool JSON::parseStatus(json_t* root, rtt::Tweet& tweet) {
	
	// text
	json_t* node =	json_object_get(root, "text");
	if(!json_is_string(node)) {
		printf("Error: cannot get text from tweet.\n");
		return false;
	}
	
	string text = json_string_value(node);
	tweet.setText(text);
	
	// id_str
	node = json_object_get(root, "id_str");
	if(!json_is_string(node)) {
		printf("Error: cannot get id_str from tweet.\n");
		return false;
	}
		
	string id = json_string_value(node);
	tweet.setTweetID(id);

	// user - object
	json_t* user = json_object_get(root, "user");
	if(!json_is_object(user)) {
		printf("Error: cannot get user node from tweet.\n");
		return false;
	}

	// user: screen name
	node = json_object_get(user, "screen_name");
	if(!json_is_string(node)) {
		printf("Error: cannot get user screen_name.\n");
		return false;
	}
	string screen_name = json_string_value(node);
	tweet.setScreenName(screen_name);
	
	// user: avatar
	node = json_object_get(user, "profile_image_url");
	if(!json_is_string(node)) {
		printf("Error: cannot get profile_image_url.\n");
		return false;
	}
	string profile_image = json_string_value(node);
	tweet.setAvatar(profile_image);

	// user: id
	node = json_object_get(user, "id_str");
	if(!json_is_string(node)) {
		printf("Error: cannot get user id.\n");
		return false;
	}
	string user_id = json_string_value(node);
	tweet.setUserID(user_id);
	return true;	
}

void JSON::parse(const string& line) {
	json_t* root;
	json_error_t error;

	// load json into jansson
	root = json_loads(line.c_str(), 0, &error);
	if(!root) {
		printf("Error: on line: %d, %s\n", error.line, error.text);
		json_decref(root);
		return;
	}
	
	// STATUS UPDATE
	json_t* node =	json_object_get(root, "text");
	if(json_is_string(node)) {
		rtt::Tweet tweet;
		if(parseStatus(root, tweet)) {
			twitter.onStatusUpdate(tweet);
		}
		json_decref(root);
		return;
	}
}

}}} // roxlu::twitter::parser