#include "JSON.h"
#include "../Twitter.h"
#include "../../../libs/curl/curl.h"
#include "../../../libs/jansson/jansson.h"
#define RETURN_JSON(msg) string m = msg; printf("Error: %s", m.c_str()); return false;

namespace rtt = roxlu::twitter::type;

namespace roxlu {
namespace twitter {
namespace parser {


JSON::JSON(roxlu::twitter::Twitter& tw) 
	:twit(tw)
{
}

JSON::JSON(const roxlu::twitter::parser::JSON& other)
	:twit(other.twit)
{
	*this = other;
}

JSON& JSON::operator=(const roxlu::twitter::parser::JSON& other) {
	if(this != &other) {
		return *this;
	}
	//twit = other.twit;
	return *this;
}


JSON::~JSON() {
	printf("~JSON()");
}

bool JSON::parseStatus(json_t* root, rtt::Tweet& tweet) {
	// text
	json_t* node =	json_object_get(root, "text");
	if(node == NULL) {
		RETURN_JSON("Error: cannot get text from tweet.\n");
	}
	if(json_is_string(node)) {
		string text = json_string_value(node);
		tweet.setText(text);
	}
	
	//printf(">  %s\n", tweet.text.c_str());
	
	// id_str
	node = json_object_get(root, "id_str");
	if(node == NULL) {
		RETURN_JSON("Error: cannot get id_str from tweet.\n");
	}
	if(json_is_string(node)) {
		string id = json_string_value(node);
		tweet.setTweetID(id);
	}
		

	// user - object
	json_t* user = json_object_get(root, "user");
	if(user == NULL || !json_is_object(user)) {
		RETURN_JSON("Error: cannot get user node from tweet.\n");
	}

	// user: screen name
	node = json_object_get(user, "screen_name");
	if(node == NULL) {
		RETURN_JSON("Error: cannot get user screen_name.\n");
	}
	if(json_is_string(node)) {
		string screen_name = json_string_value(node);
		tweet.setScreenName(screen_name);
	}
	
	// user: avatar
	node = json_object_get(user, "profile_image_url");
	if(node == NULL) {
		RETURN_JSON("Error: cannot get profile_image_url.\n");
	}
	if(json_is_string(node)) {
		string profile_image = json_string_value(node);
		tweet.setAvatar(profile_image);
	}

	// user: id
	node = json_object_get(user, "id_str");
	if(node == NULL) {
		RETURN_JSON("Error: cannot get user id.\n");
	}
	if(json_is_string(node)) {
		string user_id = json_string_value(node);
		tweet.setUserID(user_id);
	}
	
	// entities
	node = json_object_get(root, "entities");
	if(node != NULL && json_is_object(node)) {
		// get user mentions
		json_t* subnode = json_object_get(node, "user_mentions");
		if(subnode != NULL && json_is_array(subnode)) {
			size_t num = json_array_size(subnode);
			json_t* val = NULL;
			for(int i = 0; i < num; ++i) {
				val = json_array_get(subnode, i);
				if(val == NULL || !json_is_object(val)) {
					continue;
				}
				json_t* name_node = json_object_get(val, "screen_name");
				if(name_node == NULL) {
					continue;
				}
				string name_text = json_string_value(name_node);
				tweet.user_mentions.push_back(name_text);
			}
		}
		
		// get hash tags.
		subnode = json_object_get(node, "hashtags");
		if(subnode != NULL && json_is_array(subnode)) {
			size_t num = json_array_size(subnode);
			json_t* val = NULL;
			for(int i = 0; i < num; ++i) {
				val = json_array_get(subnode, i);
				if(val == NULL || !json_is_object(val)) {
					continue;
				}
				json_t* tag = json_object_get(val, "text");
				if(tag == NULL) {
					continue;
				}
				string tag_text = json_string_value(tag);
				tweet.tags.push_back(tag_text);
			}
		}
		

	}
	return true;	
}

bool JSON::parseDestroy(json_t* root, rtt::StatusDestroy& destroy) {
	json_t* del = json_object_get(root, "delete");
	if(!json_is_object(del)) {
		RETURN_JSON("Error: No delete found.\n");
	}
	
	json_t* status = json_object_get(del, "status");
	if(!json_is_object(status)) {
		RETURN_JSON("Error: No status found in delete.\n");
	}
	
	// user_id_str
	json_t* node = json_object_get(status, "user_id_str");
	if(node == NULL) {
		RETURN_JSON("Error: No user_id_str found.\n");
	}
	if(json_is_string(node)) {
		destroy.user_id_str = json_string_value(node);	
	}

	
	// id_str
	node = json_object_get(status, "id_str");
	if(node == NULL) {
		RETURN_JSON("Error: No id_str found.\n");	
	}
	if(json_is_string(node)) {
		destroy.id_str = json_string_value(node);	
	}
	
	// id
	node = json_object_get(status, "id");
	if(node == NULL) {
		RETURN_JSON("Error: no id value found.\n");
	}
	if(json_is_integer(node)) {
		destroy.id = json_integer_value(node);
	}
	
	// user_id
	node = json_object_get(status, "user_id");
	if(node == NULL) {
		RETURN_JSON("Error: no user id found.\n");
	}
	if(json_is_integer(node)) {
		destroy.user_id = json_integer_value(node);
	}
	return true;
}

/*
	string name;
	string mode;
	string description;
	User user;	
*/
bool JSON::parseStreamEvent(json_t* root, rtt::StreamEvent& event) {
	// TARGET:
	// -------------------------------------------
	json_t* node = json_object_get(root, "target");
	if(!json_is_object(node)) {
		RETURN_JSON("no stream event; target not found.\n");
	}

	if(!parseUser(node, event.target)) {
		RETURN_JSON("cannot parse target from event.\n");
	}
	
	// TARGET_OBJECT
	// -------------------------------------------
	node = json_object_get(root, "target_object");
	if(!json_is_object(node)) {
		RETURN_JSON("no target object in stream event.\n");
	}
	
	// name
	json_t* subnode = json_object_get(node, "name");
	if(!json_is_string(subnode)) {
		RETURN_JSON("no name in target object.\n");
	}
	event.target_object.name = json_string_value(subnode);
	
	// mode
	subnode = json_object_get(node, "mode");
	if(!json_is_string(subnode)) {
		RETURN_JSON("no mode in target object.\n");
	}
	event.target_object.mode = json_string_value(subnode);

	// description
	subnode = json_object_get(node, "description");
	if(!json_is_string(subnode)) {
		RETURN_JSON("no mode in target object.\n");
	}
	event.target_object.description = json_string_value(subnode);
	

	// user
	subnode = json_object_get(node, "user");
	if(!json_is_object(subnode)) {
		RETURN_JSON("no user in target object.\n");
	}
	
	if(!parseUser(subnode, event.target_object.user)) {
		RETURN_JSON("cannot parse target_object from event.\n");
	}
	
	// SOURCE
	// -------------------------------------------
	node = json_object_get(root, "source");
	if(!json_is_object(node)) {  
		RETURN_JSON("no source in stream event.\n");
	}		
	if(!parseUser(node, event.source)) {
		RETURN_JSON("cannot parse source from event.\n");
	}
	return true;
}

/*
struct User {
	string name;
	string id_str;
	string screen_name;
	string location;
	uint64_t statuses_count;
	uint64_t id;
};
*/
bool JSON::parseUser(json_t* root, rtt::User& user) {
	json_t* node = json_object_get(root, "contributors_enabled");
	if(node == NULL) {
		RETURN_JSON("given node is not a user object.\n");
	}
	if(json_is_boolean(node)) {
		user.contributors_enabled = json_is_true(node);
	}
	
	// name
	node = json_object_get(root, "name");
	if(node == NULL) {
		RETURN_JSON("no user name found.\n");
	}
	if(json_is_string(node)) {
		user.name = json_string_value(node);
	}	
	
	
	// id_str
	node = json_object_get(root, "id_str");
	if(node == NULL) {
		RETURN_JSON("no id_str found in user.\n");
	}
	if(json_is_string(node)) {
		user.id_str = json_string_value(node);
	}
	
	// screen_name
	node = json_object_get(root, "screen_name");
	if(node == NULL) {
		RETURN_JSON("no screen name found in user\n");
	}
	if(json_is_string(node)) {
		user.screen_name = json_string_value(node);
	}
	
	// location
	node = json_object_get(root, "location");
	if(node == NULL) {
		RETURN_JSON("no location in user.\n");
	}
	if(json_is_string(node)) {
		user.location = json_string_value(node);
	}
	
	
	// status count
	node = json_object_get(root, "statuses_count");
	if(node == NULL) {
		RETURN_JSON("no status count in user.\n");
	}
	if(json_is_integer(node)) {
		user.id = json_integer_value(node);
	}
	

	return true;
}


void JSON::parse(const string& line) {
	json_t* root;
	json_error_t error;
//	printf("--\n");
//	printf("%s\n\n---------------------------------------------------------\n", line.c_str());
	
	// load json into jansson
	root = json_loads(line.c_str(), 0, &error);
	if(!root) {
		printf("Error: on line: %d, %s\n", error.line, error.text);
		json_decref(root);
		return;
	}
	
	// STATUS UPDATE
	json_t* node = json_object_get(root, "text");
	if(json_is_string(node)) {
		rtt::Tweet tweet;
		if(parseStatus(root, tweet)) {
			twit.onStatusUpdate(tweet);
		}
		json_decref(root);
		return;
	}
	
	// STATUS DESTROY
	node = json_object_get(root, "delete");
	if(json_is_object(node)) {
		rtt::StatusDestroy destroy;
		if(parseDestroy(root, destroy)) {
			twit.onStatusDestroy(destroy);
		}
		//printf("found a delete\n");
		json_decref(root);
		return;
	
	}
	
	// EVENT
	node = json_object_get(root, "event");
	if(json_is_string(node)) {
		string event = json_string_value(node);
		printf("event: %s\n", event.c_str());

		if(event == "follow") {
		}
		else if(event == "list_member_added"
				|| event == "list_member_removed") 
		{
			rtt::StreamEvent stream_event;
			stream_event.event = event;
			if(parseStreamEvent(root, stream_event)) {
				twit.onStreamEvent(stream_event);
			}
		}
	}

}

}}} // roxlu::twitter::parser