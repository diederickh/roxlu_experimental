#include <algorithm>
#include "Twitter.h"

namespace roxlu {
namespace twitter {

Twitter::Twitter() 
	:json(*this)
{
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
	authURL.assign(URL_AUTHORIZE);
	authURL.append(oauth.getTokenKey());
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
	req.getParams().addString("oauth_token", oauth.getTokenKey());
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
	return true;
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
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);
	return true;
}

bool Twitter::removeTokens(const string& filePath) {
	std::remove(filePath.c_str());
}


// Event listeners
// -----------------------------------------------------------------------------
void Twitter::addEventListener(IEventListener& listener) {
	addEventListener(&listener);
}

void Twitter::addEventListener(IEventListener* listener) {
	listeners.push_back(listener);
}

void Twitter::onStatusUpdate(const rtt::Tweet& tweet) {
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onStatusUpdate(tweet);
	}
}

void Twitter::onStatusDestroy(const rtt::StatusDestroy& destroy) {
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onStatusDestroy(destroy);
	}
}

void Twitter::onStreamEvent(const rtt::StreamEvent& event) {
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onStreamEvent(event);
	}
}

// API implementation
// -----------------------------------------------------------------------------
// called just before a new API request is made.
void Twitter::reset() {
	response.clear(); 
}

// :::::::::::::::: tweets

// https://dev.twitter.com/docs/api/1/get/statuses/%3Aid/retweeted_by
bool Twitter::statusesRetweetedBy(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_BASE +tweetID +URL_STATUSES_RETWEETED_BY;
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/%3Aid/retweeted_by/ids
bool Twitter::statusesRetweetedByIDs(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_BASE +tweetID +URL_STATUSES_RETWEETED_BY_IDS;
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweets/%3Aid
bool Twitter::statusesRetweets(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_RETWEETS +tweetID +".json";
	return doGet(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/show/%3Aid
bool Twitter::statusesShow(const string& tweetID, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("id", tweetID);
	return doGet(URL_STATUSES_SHOW, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/destroy/%3Aid
bool Twitter::statusesDestroy(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_DESTROY +tweetID +".json";
	return doPost(url, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/retweet/%3Aid
bool Twitter::statusesRetweet(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_STATUSES_RETWEET +tweetID +".json";
	return doPost(url, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/update
bool Twitter::statusesUpdate(const string& tweet, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("status", tweet);
	return doPost(URL_STATUSES_UPDATE, &col, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/statuses/update_with_media
bool Twitter::statusesUpdateWithMedia(const string& tweet, const string& imageFilePath, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("status", tweet);
	col.addFile("media[]", imageFilePath);
	return doPost(URL_STATUSES_UPDATE_WITH_MEDIA, &col, true, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/oembed
bool Twitter::statusesoEmbed(const string& tweetID, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("id", tweetID);
	col.addString("url", "https://twitter.com/twitter/status/" +tweetID);
	return doGet(URL_STATUSES_OEMBED, &col, extraParams);
}


// :::::::::::::::: timeslines 

// https://dev.twitter.com/docs/api/1/get/statuses/home_timeline
bool Twitter::statusesHomeTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_HOME_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/mentions
bool Twitter::statusesMentions(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_MENTIONS, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/public_timeline
bool Twitter::statusesPublicTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_PUBLIC_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_by_me
bool Twitter::statusesRetweetedByMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETED_BY_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_to_me
bool Twitter::statusesRetweetedToMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETED_TO_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweets_of_me
bool Twitter::statusesRetweetsOfMe(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_RETWEETS_OF_ME, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/user_timeline
bool Twitter::statusesUserTimeline(rtp::Collection* extraParams) {
	return doGet(URL_STATUSES_USER_TIMELINE, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_to_user
bool Twitter::statusesRetweetedToUser(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_STATUSES_RETWEETED_TO_USER, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/statuses/retweeted_by_user
bool Twitter::statusesRetweetedByUser(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_STATUSES_RETWEETED_BY_USER, &col, extraParams);
}

// :::::::::::::::: search

// https://dev.twitter.com/docs/api/1/get/search
bool Twitter::search(const string& query, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("q", query);
	return doGet(URL_SEARCH, &col, extraParams);
}

// :::::::::::::::: direct mesages
// https://dev.twitter.com/docs/api/1/get/direct_messages
bool Twitter::directMessages(rtp::Collection* extraParams) {
	return doGet(URL_DIRECT_MESSAGES, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/direct_messages/sent
bool Twitter::directMessagesSent(rtp::Collection* extraParams) {
	return doGet(URL_DIRECT_MESSAGES_SENT, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/direct_messages/destroy/%3Aid
bool Twitter::directMessagesDestroy(const string& messageID, rtp::Collection* extraParams) {
	string url = URL_DIRECT_MESSAGES_DESTROY +messageID +".json";
	return doPost(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/direct_messages/new
bool Twitter::directMessagesNew(const string& screenName, const string& text, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	col.addString("text", text);
	return doPost(URL_DIRECT_MESSAGES_NEW, &col, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/direct_messages/show/%3Aid
bool Twitter::directMessagesShow(const string& messageID) {
	string url = URL_DIRECT_MESSAGES_SHOW +messageID +".json";
	return doGet(url);
}

// :::::::::::::::: followers & friends.

// https://dev.twitter.com/docs/api/1/get/followers/ids	
bool Twitter::followersIDs(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_FOLLOWERS_IDS, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/friends/ids
bool Twitter::friendsIDs(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_FRIENDS_IDS, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/friendships/exists
bool Twitter::friendshipsExists(const string& screenNameA, const string& screenNameB) {
	rtp::Collection col;
	col.addString("screen_name_a", screenNameA);
	col.addString("screen_name_b", screenNameB);
	return doGet(URL_FRIENDSHIPS_EXISTS, &col);
}

// https://dev.twitter.com/docs/api/1/get/friendships/incoming
bool Twitter::friendshipsIncoming(rtp::Collection* extraParams) {
	return doGet(URL_FRIENDSHIPS_INCOMING, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/friendships/outgoing
bool Twitter::friendshipsOutgoing(rtp::Collection* extraParams) {
	return doGet(URL_FRIENDSHIPS_OUTGOING, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/friendships/show
bool Twitter::friendshipsShow(const string& sourceScreenName, const string& targetScreenName) {
	rtp::Collection col;
	col.addString("source_screen_name", sourceScreenName);
	col.addString("target_screen_name", targetScreenName);
	return doGet(URL_FRIENDSHIPS_SHOW, &col);
}

// https://dev.twitter.com/docs/api/1/post/friendships/create
bool Twitter::friendshipsCreate(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doPost(URL_FRIENDSHIPS_CREATE, &col, false, extraParams);
}

//https://dev.twitter.com/docs/api/1/post/friendships/destroy
bool Twitter::friendshipsDestroy(const string& screenName, rtp::Collection* extraParams) {
	string url = URL_FRIENDSHIPS_DESTROY +screenName +".json";
	return doPost(url, extraParams, false);
}

// https://dev.twitter.com/docs/api/1/get/friendships/lookup
bool Twitter::friendshipsLookup(const string& screenNames) {
	rtp::Collection col;
	col.addString("screen_name", screenNames);
	return doGet(URL_FRIENDSHIPS_LOOKUP, &col);
}

// https://dev.twitter.com/docs/api/1/post/friendships/update
bool Twitter::friendshipsUpdate(const string& screenName, bool enableDeviceNotifications, bool enableRetweets) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	col.addString("device", (enableDeviceNotifications) ? "1" : "0");
	col.addString("retweets", (enableRetweets) ? "1" : "0");
	return doPost(URL_FRIENDSHIPS_UPDATE, &col, false);
}

// https://dev.twitter.com/docs/api/1/get/friendships/no_retweet_ids
bool Twitter::friendshipsNoRetweetIDs(bool stringifyIDs) {
	rtp::Collection col;
	col.addString("stringify_ids", (stringifyIDs) ? "1" : "0");
	return doGet(URL_FRIENDSHIPS_NO_RETWEET_IDS, &col);
}

// :::::::::::::::: users
// https://dev.twitter.com/docs/api/1/get/users/lookup
bool Twitter::usersLookup(const string& screenNames, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenNames);
	return doGet(URL_USERS_LOOKUP, &col);
}

// https://dev.twitter.com/docs/api/1/get/users/search
bool Twitter::usersSearch(const string& q, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("q", q);
	return doGet(URL_USERS_SEARCH, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/users/show
bool Twitter::usersShow(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_USERS_SHOW, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/users/contributees
bool Twitter::usersContributees(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_USERS_CONTRIBUTEES, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/users/contributors
bool Twitter::usersContributors(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_USERS_CONTRIBUTORS, &col, extraParams);
}

// :::::::::::::::: users suggestions

// https://dev.twitter.com/docs/api/1/get/users/suggestions
bool Twitter::usersSuggestions() {
	return doGet(URL_USERS_SUGGESTIONS);
}

// https://dev.twitter.com/docs/api/1/get/users/suggestions/%3Aslug
bool Twitter::usersSuggestionsSlug(const string& slug, string lang) {
	rtp::Collection col;
	string url = URL_USERS_SUGGESTIONS_SLUG +slug +".json";
	
	if(lang.length()) {
		col.addString("lang", lang);
	}
	return doGet(url, &col);
}

// https://dev.twitter.com/docs/api/1/get/users/suggestions/%3Aslug/members
bool Twitter::usersSuggestionsSlugMembers(const string& slug) {
	string url = URL_USERS_SUGGESTIONS_SLUG +slug +"/members.json";
	return doGet(url);
}

// :::::::::::::::: favorites

// https://dev.twitter.com/docs/api/1/get/favorites
bool Twitter::favorites(rtp::Collection* extraParams) {
	return doGet(URL_FAVORITES, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/favorites/create/%3Aid
bool Twitter::favoritesCreate(const string& tweetID, rtp::Collection* extraParams) {
	string url = URL_FAVORITES_CREATE +tweetID +".json";
	return doPost(url, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/favorites/destroy/%3Aid
bool Twitter::favoritesDestroy(const string& tweetID) {
	string url = URL_FAVORITES_DESTROY +tweetID +".json";
	return doPost(url, NULL, false, NULL);
}

// :::::::::::::::: lists
// https://dev.twitter.com/docs/api/1/get/lists/all
bool Twitter::listsAll(const string& screenName) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_FAVORITES, &col);
}

// https://dev.twitter.com/docs/api/1/get/lists/statuses
bool Twitter::listsStatuses(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doGet(URL_LISTS_STATUSES, &col, extraParams);	
}

// https://dev.twitter.com/docs/api/1/post/lists/members/destroy
bool Twitter::listsMembersDestroy(const string& slug, const string& ownerScreenName) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doPost(URL_LISTS_MEMBERS_DESTROY, &col, false, NULL);
}

// https://dev.twitter.com/docs/api/1/get/lists/memberships
bool Twitter::listsMemberships(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col; 
	col.addString("screen_name", screenName);
	return doGet(URL_LISTS_MEMBERSHIPS, &col);
}

// https://dev.twitter.com/docs/api/1/get/lists/subscribers
bool Twitter::listsSubscribers(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doGet(URL_LISTS_SUBSCRIBERS, &col);
}

// https://dev.twitter.com/docs/api/1/post/lists/subscribers/create
bool Twitter::listsSubscribersCreate(const string& slug, const string& ownerScreenName) {	
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doPost(URL_LISTS_SUBSCRIBERS_CREATE, &col, false, NULL);
}

// https://dev.twitter.com/docs/api/1/get/lists/subscribers/show
bool Twitter::listsSubscribersShow(
	 const string& slug
	,const string& ownerScreenName
	,const string& screenName
	,rtp::Collection* extraParams
)
{
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	col.addString("screen_name", screenName);
	return doGet(URL_LISTS_SUBSCRIBERS_SHOW, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/lists/subscribers/destroy
bool Twitter::listsSubscribersDestroy(const string& slug, const string& ownerScreenName) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	//printf("%s\n", URL_LISTS_SUBSCRIBERS_DESTROY.c_str());
	return doPost(URL_LISTS_SUBSCRIBERS_DESTROY, &col, false, NULL);
}

// https://dev.twitter.com/docs/api/1/post/lists/members/create_all
bool Twitter::listsMembersCreateAll(const string& slug, const string& ownerScreenName, const string& screenNames) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	col.addString("screen_name", screenNames);
	return doPost(URL_LISTS_MEMBERS_CREATE_ALL, &col, false, NULL);
}

// https://dev.twitter.com/docs/api/1/get/lists/members/show
bool Twitter::listsMembersShow(const string& slug, const string& ownerScreenName, const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	col.addString("screen_name", screenName);
	return doGet(URL_LISTS_MEMBERS_SHOW, &col, extraParams);
}


// https://dev.twitter.com/docs/api/1/post/lists/create	
bool Twitter::listsCreate(const string& name, bool isPublic, const string& description) {
	rtp::Collection col;
	col.addString("name", name);
	col.addString("mode", (isPublic) ? "public" : "private");
	if(description.length()) {
		col.addString("description", description);
	}
	return doPost(URL_LISTS_CREATE, &col, false, NULL);
}

// https://dev.twitter.com/docs/api/1/post/lists/update
bool Twitter::listsUpdate(
	 const string& slug
	,const string& ownerScreenName
	,bool isPublic, const string& description
)
{
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	col.addString("mode", (isPublic) ? "public" : "private");
	if(description.length()) {
		col.addString("description", description);
	}	
	return doPost(URL_LISTS_UPDATE, &col, false,  NULL);
}

// https://dev.twitter.com/docs/api/1/get/lists
bool Twitter::lists(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name",screenName);
	return doGet(URL_LISTS, &col);
}

// https://dev.twitter.com/docs/api/1/get/lists/show
bool Twitter::listsShow(const string& slug, const string& ownerScreenName) {
	rtp::Collection col;
	col.addString("owner_screen_name" , ownerScreenName);
	col.addString("slug", slug);
	return doGet(URL_LISTS_SHOW, &col);
}

// https://dev.twitter.com/docs/api/1/get/lists/subscriptions
bool Twitter::listsSubscriptions(const string& screenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("screen_name", screenName);
	return doGet(URL_LISTS_SUBSCRIPTIONS, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/get/lists/members 
bool Twitter::listsMembers(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doGet(URL_LISTS_MEMBERS, &col, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/lists/members/create
bool Twitter::listsMembersCreate(const string& slug, const string& ownerScreenName, const string& screenName) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	col.addString("screen_name", screenName);
	return doPost(URL_LISTS_MEMBERS_CREATE, &col, false, NULL);
}


// https://dev.twitter.com/docs/api/1/post/lists/destroy
bool Twitter::listsDestroy(const string& slug, const string& ownerScreenName) {
	rtp::Collection col;
	col.addString("slug", slug);
	col.addString("owner_screen_name", ownerScreenName);
	return doPost(URL_LISTS_DESTROY, &col, false, NULL);
}


// :::::::::::::::: account

// https://dev.twitter.com/docs/api/1/get/account/rate_limit_status
bool Twitter::accountRateLimitStatus() {	
	return doGet(URL_ACCOUNT_RATE_LIMIT_STATUS);
}


// https://dev.twitter.com/docs/api/1/get/account/verify_credentials
bool Twitter::accountVerifyCredentials(rtp::Collection* extraParams) {
	return doGet(URL_ACCOUNT_VERIFY_CREDENTIALS);
}

// https://dev.twitter.com/docs/api/1/post/account/end_session
bool Twitter::accountEndSession() {
	return doPost(URL_ACCOUNT_END_SESSION, NULL, false, NULL);
}

// https://dev.twitter.com/docs/api/1/post/account/update_profile
bool Twitter::accountUpdateProfile(
	 const string& name
	,const string& url
	,const string& location
	,const string& description
	,rtp::Collection* extraParams
)
{
	rtp::Collection col;
	if(name.length()) {
		col.addString("name", name);
	}
	if(url.length()) {
		col.addString("url", url);
	}
	if(location.length()) {
		col.addString("location", location);
	}
	if(description.length()) {
		col.addString("description", description);
	}
	return doPost(URL_ACCOUNT_UPDATE_PROFILE, &col, false, extraParams);
}


// https://dev.twitter.com/docs/api/1/post/account/update_profile_colors
bool Twitter::accountUpdateProfileColors(
	 const string& bgColor
	,const string& profileLinkColor
	,const string& sidebarBorderColor
	,const string& sidebarFillColor
	,const string& profileTextColor
	,rtp::Collection* extraParams
)
{
	rtp::Collection col;
	if(bgColor.length()) {
		col.addString("profile_background_color", bgColor);
	}
	if(profileLinkColor.length()) {
		col.addString("profile_link_color", profileLinkColor);
	}
	if(sidebarBorderColor.length()) {
		col.addString("profile_sidebar_border_color", sidebarBorderColor);
	}
	if(sidebarFillColor.length()) {
		col.addString("profile_sidebar_fill_color", sidebarFillColor);
	}
	if(profileTextColor.length()) {
		col.addString("profile_text_color", profileTextColor);
	}
	return doPost(URL_ACCOUNT_UPDATE_PROFILE_COLORS, &col, false, extraParams);
}

// https://dev.twitter.com/docs/api/1/post/account/update_profile_background_image
bool Twitter::accountUpdateProfileBackgroundImage(
	 const string& imageFilePath
	,bool tile
	,bool use
	,rtp::Collection* extraParams
)
{
	rtp::Collection col;
	if(imageFilePath.length()) {
		col.addFile("image", imageFilePath);
	}
	col.addString("tile", (tile) ? "1" : "0");
	col.addString("use", (use) ? "1" : "0");

	if(imageFilePath.length()) {
		return doPost(URL_ACCOUNT_UPDATE_PROFILE_BACKGROUND_IMAGE, &col, true, extraParams);
	}
	else {
		return doPost(URL_ACCOUNT_UPDATE_PROFILE_BACKGROUND_IMAGE, &col, false, extraParams);	
	}
}

// https://dev.twitter.com/docs/api/1/post/account/update_profile_image
bool Twitter::accountUpdateProfileImage(const string& imageFilePath) {
	rtp::Collection col;
	col.addFile("image", imageFilePath);
	return doPost(URL_ACCOUNT_UPDATE_PROFILE_IMAGE, &col, true, NULL);
}

// https://dev.twitter.com/docs/api/1/get/account/totals
bool Twitter::accountTotals() {
	return doGet(URL_ACCOUNT_TOTALS);
}

// https://dev.twitter.com/docs/api/1/get/account/settings
bool Twitter::accountSettings() {
	return doGet(URL_ACCOUNT_SETTINGS);
}

}} // roxlu::twitter