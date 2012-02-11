#include "TwitPic.h"
#include "Request.h"
#include "ofMain.h"
#include "Curl/Curl.h"
namespace roxlu {
namespace twitter {

TwitPic::TwitPic(const string& key, Twitter& twitter)
	:twitter(twitter)
	,key(key)
{
}

TwitPic::TwitPic(const TwitPic& other)
	:twitter(other.twitter)
	,key(other.key)
{
}

TwitPic::~TwitPic() {
}

TwitPic& TwitPic::operator=(const TwitPic& other) {
	if(this == &other) {
		return *this;
	}
	twitter = other.twitter;
	key = other.key;
	return *this;
}


bool TwitPic::upload(const string& filePath, TwitPicResult& result, const string& message) {
	// Create a request which is used to generate the credentials oauth header.
	rc::Request req(URL_ACCOUNT_VERIFY_CREDENTIALS);
	req.isGet(true);
	twitter.getoAuth().updateNonce();
	string oauth_header = twitter.getoAuth().getHeader(req, req.getParams());
	
	// Now use the oauth header for the credentials so twitpic can validate us.
	req.setURL("http://api.twitpic.com/2/upload.json");
	req.addHeader("X-Verify-Credentials-Authorization: " +oauth_header);
	req.addHeader("X-Auth-Service-Provider: https://api.twitter.com/1/account/verify_credentials.json");
	
	// Add some fields to the post.
	req.getParams().addFile("media", filePath);
	req.getParams().addString("key", key);
	req.getParams().addString("message", message);
	
	// Use curl to perform a post.
	roxlu::curl::Curl rox_curl;
	string json;
	req.doPost(rox_curl, json, true);
		
	return parseJSON(json, result);
}


bool TwitPic::parseJSON(const string& json, TwitPicResult& result) {
	printf("result:\n%s\n", json.c_str());
	json_t* root;
	json_error_t error;
	
	root = json_loads(json.c_str(), 0, &error);
	if(!root) {
		printf("Error: on line: %d, %s\n", error.line, error.text);
		json_decref(root);
		return false;
	}
	
	// id
	json_t* node = json_object_get(root, "id");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no id found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_string(node)) {
		result.id_str = json_string_value(node);
	}
	
	// text
	node = json_object_get(root, "text");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no text found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_string(node)) {
		result.text = json_string_value(node);
	}

	// url
	node = json_object_get(root, "url");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no url found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_string(node)) {
		result.url = json_string_value(node);
	}
	
	// width
	node = json_object_get(root, "width");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no width found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_integer(node)) {
		result.width = json_integer_value(node);
	}

	// height
	node = json_object_get(root, "height");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no height found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_integer(node)) {
		result.height = json_integer_value(node);
	}
	
	// size
	node = json_object_get(root, "size");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no size found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_integer(node)) {
		result.size = json_integer_value(node);
	}

	// type
	node = json_object_get(root, "type");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no type found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_string(node)) {
		result.type = json_string_value(node);
	}
	
	// timestamp
	node = json_object_get(root, "timestamp");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no timestamp found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_string(node)) {
		result.timestamp = json_string_value(node);
	}
	
	// user
	// --------------
	json_t* subnode = json_object_get(root, "user");
	if(node == NULL) {
		printf("Error: cannot parse twitpic result, no user found.\n");
		json_decref(root);
		return false;
	}
	if(json_is_object(subnode)) {
		// user_id
		node = json_object_get(subnode, "id");
		if(node == NULL) {
			printf("Error: cannot parse twitpic result, no user id found.\n");
			json_decref(root);
			return false;
		}
		if(json_is_integer(node)) {
			result.user_id = json_integer_value(node);
		}
		
		// user_screen_name
		node = json_object_get(subnode, "screen_name");
		if(node == NULL) {
			printf("Error: cannot parse twitpic result, no user screen_name found.\n");
			json_decref(root);
			return false;
		}
		if(json_is_string(node)) {
			result.user_screen_name = json_string_value(node);
		}
	}
	result.print();
	return true;
}


/*
	//twitter.getoAuth().authorize(req);
	printf("\n\n\n------------------------------------\n\n");
	printf("Header: %s\n", oauth_header.c_str());
	printf("\n\n++++++++++++++++++++++++++++++++++++++\n\n");
	printf("%s\n", result.c_str());
	printf("\n\n++++++++++++++++++++++++++++++++++++++\n\n");

*/

}} // roxlu::twitter