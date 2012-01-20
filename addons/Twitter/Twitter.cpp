#include "Twitter.h"
#include "TwitteroAuthSignature.h"
#include "TwitteroAuthHeader.h"

namespace roxlu {

Twitter::Twitter() 
	:curl_param_callback_set(false)
	,curl_param_login_set(false)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(curl == NULL) {
		printf("Error: cannot initialize curl for twitter.\n");
	}
}

Twitter::~Twitter() {
}

bool Twitter::requestToken(string& authURL /* out */) {
	// get request-token.
	string response;
	rtc::Request req = oauth.getRequestTokenRequest();
	if(!req.doGet(twitcurl, response)) {
		printf("error: cannot do request for token.\n");
		return false;
	}
	
	// extract request-token and secret from result.
	string token;
	string secret;
	if(!rto::Utils::extractTokenAndSecret(response, token, secret)) {
		return false;
	}
	oauth.setTokenKey(token);
	oauth.setTokenSecret(secret);
	//printf("token: %s secret: %s\n", token.c_str(), secret.c_str());
	// use these token and secret to create the authorization url.
	authURL.assign("http://twitter.com/oauth/authorize?oauth_token=");
	authURL.append(oauth.getTokenKey());
	return true;

	/*
	if(!isCurlInitialized()) {
		return false;
	}

	// Get header with all the necessary oauth values
	// @see https://dev.twitter.com/docs/auth/oauth	
	string header;
	string url = "http://twitter.com/oauth/request_token";
	bool result = false;
	result = auth.getHeader(TwitteroAuth::TWITTER_OAUTH_GET, url, NULL, header, false);
	if(!result) {
		return false;
	}
	printf("*******************************************************\n");
	printf("%s\n", header.c_str());
	printf("*******************************************************\n");
	return true;
	// get the token key and secret	
	if(performGet(url, header)) {
		if(!auth.extractTokenKeyAndSecret(buffer)) {
			printf("Error while extracting token key and secrect from response");
			return false;
		}
		authURL.assign("http://twitter.com/oauth/authorize?oauth_token=");
		authURL.append(auth.getTokenKey());
	}
	return true;
	*/
}


bool Twitter::performGet(const string& url, const vector<TwitterCurlValueType*>& keyValues) {
	string new_url = url;
	vector<TwitterCurlValueType*>::const_iterator it = keyValues.begin();
	if(keyValues.size() > 0) {
		new_url += "?";
	}
	while(it != keyValues.end()) {
		(*it)->addToGet(new_url);
		++it;
		if(it != keyValues.end()) {
			new_url += "&";
		}
	}
	printf("Created url: %s\n", new_url.c_str());
	return performGet(new_url);
}


bool Twitter::performGet(const string& url, const string& oAuthHeader) {
	struct curl_slist* header_list = NULL;
	
	prepareCurlStandardParams();
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
	// append oauth headers to http-headers
	if(oAuthHeader.length()) {
		header_list = curl_slist_append(header_list, oAuthHeader.c_str());
		if(header_list) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		}
	}
	
	CURLcode r = curl_easy_perform(curl);
	
	if(header_list) {
		curl_slist_free_all(header_list);
	}
	return (r == CURLE_OK);
}

bool Twitter::performGet(const string& url) {
	struct curl_slist* header_list = NULL;
	string dummy_data;
	string oauth_header;
	
	prepareCurlStandardParams();
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		
	/*
	auth.getHeader(TwitteroAuth::TWITTER_OAUTH_GET, url, dummy_data, oauth_header);
	if(!oauth_header.length()) {
		printf("Error: cannot create oauth header.\n");
		return false;
	}
	printf("Header: %s\n", oauth_header.c_str());
	// append oauth headers to http-headers
	header_list = curl_slist_append(header_list, oauth_header.c_str());
	if(header_list) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
	}
	
	CURLcode r = curl_easy_perform(curl);
	
	if(header_list) {
		curl_slist_free_all(header_list);
	}
	return (r == CURLE_OK);
	*/
	return true;
}


bool Twitter::performPost(const string& url, const vector<TwitterCurlValueType*>& keyValues) {
	string oauth_header;
	struct curl_slist* header_list = NULL;
	struct curl_httppost* post_curr = NULL;
	struct curl_httppost* post_last = NULL;
	CURLcode curl_result;
	
	prepareCurlStandardParams();
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	
	// add values to the post.	
	vector<TwitterCurlValueType*>::const_iterator it = keyValues.begin();
	while(it != keyValues.end()) {
		(*it)->addToPost(curl, &post_curr, &post_last);
		++it;
	}
	
	// add oauth header.
	bool result = auth.getHeader(TwitteroAuth::TWITTER_OAUTH_POST, url, NULL, oauth_header, false);
	//bool result = auth.getHeader(TwitteroAuth::TWITTER_OAUTH_POST, url, "", oauth_header);
	if(!result) {
		printf("Error: cannot create oauth header.\n");
		return false;
	}
	
	if(oauth_header.length()) {
		header_list = curl_slist_append(header_list, oauth_header.c_str());
		
		// remove expect (when getting 417 errors)
		static const char buf[] = "Expect:";
    	header_list = curl_slist_append(header_list,buf);
		
		if(header_list) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		}
	}

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // no CA verification
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_curr);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
	// perform post
	curl_result = curl_easy_perform(curl);
	if(curl_result == CURLE_OK) {
		if(header_list) {
			curl_slist_free_all(header_list);
			curl_formfree(post_curr);
			return true;
		}
	}
	else {
		printf("Error: %s\n", curl_easy_strerror(curl_result));
	}
	if(header_list) {
		curl_formfree(post_curr);
		curl_slist_free_all(header_list);
	}
	return false;
}


bool Twitter::setPin(const string& pin) {
	auth.setPin(pin);
	return true;
}

// Get pin which authorizes the application.
// -----------------------------------------
bool Twitter::handlePin(const string& authURL) {

	// STEP 1: get PIN-wise authorization
	// -------------------------------------------------------------------------
	string response;
	rtc::Request req;
	req.setURL(authURL);
	if(!req.doGet(twitcurl, response)) {
		printf("error: cannot do request for token.\n");
		return false;
	}
	
	// get authenticity token from authorization page. 
	string authenticity_token;
	rto::Utils::extractAuthenticityToken(response, authenticity_token);
	
	// STEP 2: do a post with the fields on this pin auth page.
	// -------------------------------------------------------------------------
	req.getParams().addString("oauth_token", auth.getTokenKey());
	req.getParams().addString("authenticity_token", authenticity_token);
	req.getParams().addString("session[username_or_email]", twitcurl.getAuthUsername());
	req.getParams().addString("session[password]", twitcurl.getAuthPassword());
	
	if(!req.doPost(twitcurl, response)) {
		printf("error: cannot get pin html\n");
		return false;
	}
	
	string pin;
	if(!rto::Utils::extractPin(response, pin)) {	
		printf("error: cannot extract pin.\n");
		return false;
	}
	oauth.setPin(pin);
	return true;
	
	printf("=====================================\n");
//	printf("%s\n", response.c_str());
	printf("=====================================\n");

	/*
	data_str = 	roxlu::twitter::TOKEN_KEY 				+"=" +oauth_token 			+"&" + \
				roxlu::twitter::AUTHENTICITY_TOKEN_KEY 	+"=" +oauth_authenticity 	+"&" + \
				roxlu::twitter::SESSIONUSERNAME_KEY		+"=" +getTwitterUsername()	+"&" + \
				roxlu::twitter::SESSIONPASSWORD_KEY		+"=" +getTwitterPassword();
	*/
	return true;

	string data_str;
	string oauth_header;
	string oauth_token;
	string oauth_authenticity;
	string pincode;
	unsigned long http_status = 0;
	size_t npos_start, npos_end;
	struct curl_slist* header_list = NULL;
	
	prepareCurlStandardParams();
		
	// STEP 1: load HTML and get "authenticity_token" and "oauth_token"
	// -------------------------------------------------------------------------
	auth.getHeader(TwitteroAuth::TWITTER_OAUTH_GET, authURL, NULL, oauth_header, false);
	//auth.getHeader(TwitteroAuth::TWITTER_OAUTH_GET, authURL, data_str, oauth_header);
	
	if(oauth_header.length()) {
		header_list = curl_slist_append(header_list, oauth_header.c_str());
		if(header_list) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		}
	}
	
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(curl, CURLOPT_URL, authURL.c_str());
	
	if(curl_easy_perform(curl) == CURLE_OK) {
		// parse the received HTML and get some hidden fields.
		if(header_list) {
			curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_status);
					
			// get: <input name="authenticity_token" type="hidden" value="11187c853df9ca61369472e70f5652ddc2922333" />
			npos_start = buffer.find(roxlu::twitter::AUTHENTICITY_TOKEN_TWITTER_RESP_KEY);
			npos_start += roxlu::twitter::AUTHENTICITY_TOKEN_TWITTER_RESP_KEY.length();
			npos_end = buffer.substr(npos_start).find(roxlu::twitter::TOKEN_END_TAG_TWITTER_RESP);
			oauth_authenticity = buffer.substr(npos_start, npos_end);
			
			// get: <input id="oauth_token" name="oauth_token" type="hidden" value="111IqQXJPvvXwisQKSQXEfbB463hcktCkBTWb7NcZFY" />
			npos_start = buffer.find(roxlu::twitter::TOKEN_TWITTER_RESP_KEY);
			npos_start += roxlu::twitter::TOKEN_TWITTER_RESP_KEY.length();
			npos_end = buffer.substr(npos_start).find(roxlu::twitter::TOKEN_END_TAG_TWITTER_RESP);
			oauth_token = buffer.substr(npos_start, npos_end);
			printf("oauth_token: '%s' oauth_authenticity: '%s'\n", oauth_token.c_str(), oauth_authenticity.c_str());
			curl_slist_free_all(header_list);
		}
	}
	else if(header_list) {
		curl_slist_free_all(header_list);
		return false;
	}
	
	
	// STEP 2: send a http-post to twitter including username/password and extract the PIN from the response
	// -----------------------------------------------------------------------------------------------------
	header_list = NULL;
	oauth_header.clear();
	prepareCurlStandardParams();
	
	// make sure the oauth token is added to the header
	map<string, string> extra_values;
	extra_values[roxlu::twitter::TOKEN_KEY] = oauth_token;
	
	// we also need to post these
	data_str = 	roxlu::twitter::TOKEN_KEY 				+"=" +oauth_token 			+"&" + \
				roxlu::twitter::AUTHENTICITY_TOKEN_KEY 	+"=" +oauth_authenticity 	+"&" + \
				roxlu::twitter::SESSIONUSERNAME_KEY		+"=" +getTwitterUsername()	+"&" + \
				roxlu::twitter::SESSIONPASSWORD_KEY		+"=" +getTwitterPassword();
	

	auth.getHeader(TwitteroAuth::TWITTER_OAUTH_POST, authURL, &extra_values, oauth_header, false);
	int i = 0;
	if(oauth_header.length()) {
		header_list = curl_slist_append(header_list, oauth_header.c_str());
		if(header_list) {
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
		}
	}
	
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_URL, authURL.c_str());
	curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data_str.c_str());
	
	if(curl_easy_perform(curl) == CURLE_OK) {
		int i = 0;
		if(header_list) {
			curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_status);
			npos_start = buffer.find(roxlu::twitter::PIN_TWITTER_RESP_KEY);
			npos_start += roxlu::twitter::PIN_TWITTER_RESP_KEY.length();
			npos_end = buffer.substr(npos_start).find(roxlu::twitter::PIN_END_TAG_TWITTER_RESP);
			pincode = buffer.substr(npos_start, npos_end);
			printf("Got pincode: %s\n", pincode.c_str());
			auth.setPin(pincode);
			curl_slist_free_all(header_list);
			return true;
		}
	}
	else if(header_list) {
		curl_slist_free_all(header_list);	
	}
	
	return false;
}

bool Twitter::accessToken() {
	// get request with correct headers (and signature)
	string response; 
	rtc::Request req = oauth.getAccessTokenRequest();
	if(!req.doGet(twitcurl, response)) {
		return false;
	}
	
	// extract token and secrect from response.
	string token;
	string secret;
	if(!rto::Utils::extractTokenAndSecret(response, token, secret)) {
		return false;
	}
	oauth.setTokenKey(token);
	oauth.setTokenSecret(secret);
	printf("response: '%s'\n", response.c_str());
	printf("token: '%s' secrect:'%s'\n", token.c_str(), secret.c_str());
	return true;
	/*
	if(!isCurlInitialized()) {
		printf("Error: cannot get accesstoken, curl not initialized.\n");	
		return false;
	}
	string auth_header;
	bool result = auth.getHeader(	
							 TwitteroAuth::TWITTER_OAUTH_GET
							,roxlu::twitter::ACCESS_TOKEN_URL
							,NULL
							,auth_header
							,true 
						);
	
	if(!result) {
		printf("Error: cannot get accesstoken, error retrieving auth header.\n");
		return false;
	}
	
	if(performGet(roxlu::twitter::ACCESS_TOKEN_URL, auth_header)) {
		auth.extractTokenKeyAndSecret(buffer);
		return true;
	}
	return false;
	*/
}

// Save retrieved tokens which gives the application access to the twitter account
// -------------------------------------------------------------------------------
bool Twitter::saveTokens(const string& filePath) {
	std::ofstream of(filePath.c_str());
	if(!of.is_open()) {
		printf("Error: cannot open: '%s'.\n", filePath.c_str());
		return false;
	}
	of << oauth.getTokenKey() << std::endl;
	of << oauth.getTokenSecret() << std::endl;
	of.close();
	return true;
}

bool Twitter::loadTokens(const string& filePath) {
	std::ifstream ifs(filePath.c_str());
	if(!ifs.is_open()) {
		printf("Error: cannot open: '%s'.\n", filePath.c_str());
		return false;
	}
	string token_key, token_secret;
	std::getline(ifs, token_key);
	std::getline(ifs, token_secret);
	printf("Loaded token_key='%s' token_secret='%s'\n", token_key.c_str(), token_secret.c_str());
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);
	return true;
}

bool Twitter::removeTokens(const string& filePath) {
	std::remove(filePath.c_str());
}


// Curl internals
// -----------------------------------------------------------------------------
void Twitter::prepareCurlCallback() {
	if(!curl_param_callback_set) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		curl_param_callback_set = true;
	}
}

void Twitter::prepareCurlUserPass() {
	if(!curl_param_login_set) {
		string userpass = twitter_username +":" +twitter_password;
		curl_easy_setopt(curl, CURLOPT_USERPWD, NULL); // resets current values
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass.c_str());
		curl_param_login_set = true;
	}
}

void Twitter::prepareCurlStandardParams() {
	buffer = "";
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL); // reset request, from twitcurl.
	prepareCurlCallback();
	prepareCurlUserPass();
}

size_t Twitter::curlCallback(char* data, size_t size, size_t nmemb, Twitter* twit) {
	size_t handled = size * nmemb;
	twit->buffer.append(data, handled);
	return handled;
}


// API implementation
// API doc: https://dev.twitter.com/docs/api/ 
// -----------------------------------------------------------------------------
bool Twitter::statusUpdate(const string& tweet) {
	
	// Create an authorized request object.
	rtp::Collection col;
	col["status"] = "nice nice";
	string url = "http://api.twitter.com/1/statuses/update.json";
	rtc::Request req = oauth.getAuthorizedPost(url, col);
	
	// perform the post
	string response;
	if(!req.doPost(twitcurl, response, col)) {
		printf("error: cannot do stauts update\n");
		return false;
	}
	return true;
	
	
	
	
	
	/*
		printf("-===============--\n");
	printf("%s\n", response.c_str());
	printf("-===============--\n");

	Deze wordt gemaakt voor de tweet met tekst: and another one...
	Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw", oauth_nonce="13269270842b5", oauth_signature="%2FpYiDEdeQc%2BJgOpJurMsfjg%2BMZA%3D", oauth_signature_method="HMAC-SHA1", oauth_timestamp="1326927084", oauth_token="466622389-Osbd3Mm1SDVLOqugzCQ5y6MP1RkLMw81VIREB5NR", oauth_version="1.0"
	Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw", oauth_nonce="1326928753240", oauth_signature_method="HMAC-SHA1", oauth_timestamp="1326928753", oauth_version="1.0", status="and another one..."
	Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw", oauth_nonce="", oauth_signature_method="HMAC-SHA1", oauth_timestamp="", oauth_version="1.0", status="and another one..."
	*/
	
	

/*
	if(tweet.length() && isCurlInitialized()) {
		TwitterCurlValueTypeString param_status("status", tweet);
		vector<TwitterCurlValueType*> params;
		params.push_back(&param_status);
		string url = "http://api.twitter.com/1/statuses/update.json";
		if(!performPost(url, params)) {
			return false;
		}
		
		TwitteroAuthSignature sig(auth);
	sig.addDefaultParameters();
	sig.addParameter("status", tweet);
	TwitteroAuthHeader head;
	string h = head.getHeader(sig.getParameters());
	printf("Generated header sig: %s\n", h.c_str());
		return true;
	}
	return false;
	*/
}

bool Twitter::statusUpdateWithMedia(const string& tweet, const string& imageFilePath) {
	if(tweet.length() && isCurlInitialized()) {
		vector<TwitterCurlValueType*> params;
		
		TwitterCurlValueTypeString param_status("status", tweet);
		TwitterCurlValueTypeFile param_media("media[]", imageFilePath);
		params.push_back(&param_status);
		params.push_back(&param_media);
		string url = "https://upload.twitter.com/1/statuses/update_with_media.json";
		if(!performPost(url, params)) {
			printf("Error: cannot update status with media.\n");
			return false;
		}
		return true;
	}
	return false;
}

bool Twitter::getHomeTimeline(unsigned int count) {
	// bound check.
	if(count <= 0) {
		count = 5;
	}
	else if(count > 200) {
		count = 200;
	}
	
	
	// Create an authorized request object.
	rtp::Collection col;
	col["count"] = count;
	string url = "http://api.twitter.com/1/statuses/home_timeline.json";
	rtc::Request req = oauth.getAuthorizedGet(url, col);
	//http://api.twitter.com/1/statuses/home_timeline.format
	// perform the post
	string response;
	if(!req.doGet(twitcurl, response, col)) {
		printf("error: cannot do stauts update\n");
		return false;
	}
	printf("-------------------\n");
	printf("%s\n", response.c_str());
	printf("-------------------\n");
	return true;
/*



	// bound check.
	if(count <= 0) {
		count = 5;
	}
	else if(count > 200) {
		count = 200;
	}
	
	TwitterRequest req(auth);
	req.addString("count", count, true);
	req.setURL("http://api.stwitter.com/1/statuses/home_timeline.json");
	string header = req.getHeader(false);
	printf("header:%s\n", header.c_str());
	
	TwitterCurlValues values;
	values.addString("count", count);
	string url = "http://api.stwitter.com/1/statuses/home_timeline.json";
	if(!performGet(url, values.getValues())) {
		printf("Error: cannot get timeline\n");
		return false;
	}
	return true;
	*/

}

/*
Authorization: OAuth oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw",oauth_nonce="1326838895331",oauth_signature="0OIWQm7ghWM3EpG8QjS435h817E%3D",oauth_signature_method="HMAC-SHA1",oauth_timestamp="1326838895",oauth_token="466622389-Osbd3Mm1SDVLOqugzCQ5y6MP1RkLMw81VIREB5NR",oauth_version="1.0"
Authorization: OAuth count="20",oauth_consumer_key="kyw8bCAWKbkP6e1HMMdAvw",oauth_nonce="13268385811a3",oauth_signature="jHb0eLasJ506UaeeOE%2FLrGzK0tU%3D",oauth_signature_method="HMAC-SHA1",oauth_timestamp="1326838581",oauth_token="466622389-Osbd3Mm1SDVLOqugzCQ5y6MP1RkLMw81VIREB5NR",oauth_version="1.0"
*/

} // roxlu