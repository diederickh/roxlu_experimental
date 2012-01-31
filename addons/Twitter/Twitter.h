#ifndef ROXLU_TWITTERH
#define ROXLU_TWITTERH

#include <vector>
#include <string>
#include <fstream>

#include "../../libs/curl/curl.h"
#include "../../libs/crypto/urlencode.h"

#include "parameter/Collection.h"
#include "parameter/Parameter.h"
#include "curl/Request.h"
#include "curl/Curl.h"
#include "oauth/Utils.h"
#include "oauth/oAuth.h"
#include "types/Tweet.h"
#include "types/General.h"
#include "parser/JSON.h"
#include "IEventListener.h"
#include "Stream.h"

namespace rtc = roxlu::twitter::curl;
namespace rtp = roxlu::twitter::parameter;
namespace rto = roxlu::twitter::oauth;
namespace rtt = roxlu::twitter::type;

using std::string;
using std::vector;


// This implementation is based on twitcurl with some modifications for OF
// All hard work goes to the author of TwitCurl!

namespace roxlu {
namespace twitter {

const string URL_TWITTER_BASE = "https://api.twitter.com/";
const string URL_STATUSES_BASE = URL_TWITTER_BASE +"1/statuses/";
const string URL_FRIENDSHIPS_BASE = URL_TWITTER_BASE +"1/friendships/";
const string URL_DIRECT_MESSAGES_BASE = URL_TWITTER_BASE +"1/direct_messages/";
const string URL_FOLLOWERS_BASE = URL_TWITTER_BASE +"1/followers/";
const string URL_USERS_BASE = URL_TWITTER_BASE +"1/users/";
const string URL_FAVORITES_BASE = URL_TWITTER_BASE + "1/favorites/";
const string URL_LISTS_BASE = URL_TWITTER_BASE +"1/lists/";
const string URL_ACCOUNT_BASE = URL_TWITTER_BASE +"1/account/";

const string URL_STATUSES_RETWEETED_BY = "/retweeted_by.json";
const string URL_STATUSES_RETWEETED_BY_IDS = "/retweeted_by/ids.json";
const string URL_STATUSES_RETWEETS = URL_STATUSES_BASE +"retweets/";
const string URL_STATUSES_SHOW = URL_STATUSES_BASE +"show.json";
const string URL_STATUSES_DESTROY = URL_STATUSES_BASE +"destroy/";
const string URL_STATUSES_RETWEET = URL_STATUSES_BASE +"retweet/";
const string URL_STATUSES_UPDATE = URL_STATUSES_BASE +"update.json";
const string URL_STATUSES_UPDATE_WITH_MEDIA = URL_STATUSES_BASE +"update_with_media.json";
const string URL_STATUSES_OEMBED = URL_STATUSES_BASE +"oembed.json";

const string URL_STATUSES_HOME_TIMELINE = URL_STATUSES_BASE +"home_timeline.json";
const string URL_STATUSES_MENTIONS = URL_STATUSES_BASE +"mentions.json";
const string URL_STATUSES_PUBLIC_TIMELINE = URL_STATUSES_BASE +"public_timeline.json";
const string URL_STATUSES_RETWEETED_BY_ME = URL_STATUSES_BASE +"retweeted_by_me.json";
const string URL_STATUSES_RETWEETED_TO_ME = URL_STATUSES_BASE +"retweeted_to_me.json";
const string URL_STATUSES_RETWEETS_OF_ME = URL_STATUSES_BASE +"retweets_of_me.json";
const string URL_STATUSES_USER_TIMELINE = URL_STATUSES_BASE +"user_timeline.json";
const string URL_STATUSES_RETWEETED_TO_USER = URL_STATUSES_BASE +"retweeted_to_user.json";
const string URL_STATUSES_RETWEETED_BY_USER = URL_STATUSES_BASE +"retweeted_by_user.json";

const string URL_SEARCH = "https://search.twitter.com/search.json";

const string URL_DIRECT_MESSAGES = URL_TWITTER_BASE +"1/direct_messages.json";
const string URL_DIRECT_MESSAGES_SENT = URL_DIRECT_MESSAGES_BASE +"sent.json";
const string URL_DIRECT_MESSAGES_DESTROY = URL_DIRECT_MESSAGES_BASE +"destroy/";
const string URL_DIRECT_MESSAGES_NEW = URL_DIRECT_MESSAGES_BASE +"new.json";
const string URL_DIRECT_MESSAGES_SHOW = URL_DIRECT_MESSAGES_BASE +"show/"; 

const string URL_FOLLOWERS_IDS = URL_FOLLOWERS_BASE +"ids.json";
const string URL_FRIENDS_BASE = URL_TWITTER_BASE +"1/friends/";
const string URL_FRIENDS_IDS = URL_FRIENDS_BASE +"ids.json";
const string URL_FRIENDSHIPS_EXISTS = URL_FRIENDSHIPS_BASE +"exists.json";
const string URL_FRIENDSHIPS_INCOMING = URL_FRIENDSHIPS_BASE +"incoming.json";
const string URL_FRIENDSHIPS_OUTGOING = URL_FRIENDSHIPS_BASE +"outgoing.json";
const string URL_FRIENDSHIPS_SHOW = URL_FRIENDSHIPS_BASE +"show.json";
const string URL_FRIENDSHIPS_CREATE = URL_FRIENDSHIPS_BASE +"create.json";
const string URL_FRIENDSHIPS_DESTROY = URL_FRIENDSHIPS_BASE +"destroy/";
const string URL_FRIENDSHIPS_LOOKUP = URL_FRIENDSHIPS_BASE +"lookup.json";
const string URL_FRIENDSHIPS_UPDATE = URL_FRIENDSHIPS_BASE +"update.json";
const string URL_FRIENDSHIPS_NO_RETWEET_IDS = URL_FRIENDSHIPS_BASE +"no_retweet_ids.json";

const string URL_USERS_LOOKUP = URL_USERS_BASE +"lookup.json";
const string URL_USERS_SEARCH = URL_USERS_BASE +"search.json";
const string URL_USERS_SHOW = URL_USERS_BASE +"show.json";
const string URL_USERS_CONTRIBUTEES = URL_USERS_BASE +"contributees.json";
const string URL_USERS_CONTRIBUTORS = URL_USERS_BASE +"contributors.json";
const string URL_USERS_SUGGESTIONS = URL_USERS_BASE +"suggestions.json";
const string URL_USERS_SUGGESTIONS_SLUG = URL_USERS_BASE +"suggestions/";

const string URL_FAVORITES = URL_TWITTER_BASE +"1/favorites.json";
const string URL_FAVORITES_CREATE = URL_FAVORITES_BASE +"create/";
const string URL_FAVORITES_DESTROY = URL_FAVORITES_BASE +"destroy/";

const string URL_LISTS_ALL = URL_LISTS_BASE +"all.json";
const string URL_LISTS_STATUSES = URL_LISTS_BASE +"statuses.json";
const string URL_LISTS_MEMBERS_DESTROY = URL_LISTS_BASE +"destroy.json"; // @todo
const string URL_LISTS_MEMBERSHIPS = URL_LISTS_BASE +"memberships.json";
const string URL_LISTS_SUBSCRIBERS = URL_LISTS_BASE +"subscribers.json";
const string URL_LISTS_SUBSCRIBERS_CREATE = URL_LISTS_BASE +"subscribers/create.json";
const string URL_LISTS_SUBSCRIBERS_SHOW = URL_LISTS_BASE +"subscribers/show.json";
const string URL_LISTS_SUBSCRIBERS_DESTROY = URL_LISTS_BASE +"subscribers/destroy.json";
const string URL_LISTS_MEMBERS_CREATE_ALL = URL_LISTS_BASE +"members/create_all.json";
const string URL_LISTS_MEMBERS_SHOW = URL_LISTS_BASE +"members/show.json";
const string URL_LISTS_MEMBERS_CREATE = URL_LISTS_BASE +"members/create.json";
const string URL_LISTS_CREATE = URL_LISTS_BASE +"create.json";
const string URL_LISTS_UPDATE = URL_LISTS_BASE +"update.json";
const string URL_LISTS_DESTORY = URL_LISTS_BASE +"destroy.json";
const string URL_LISTS = URL_TWITTER_BASE +"1/lists.json";
const string URL_LISTS_SHOW = URL_LISTS_BASE +"show.json";
const string URL_LISTS_SUBSCRIPTIONS = URL_LISTS_BASE +"subscriptions.json";
const string URL_LISTS_MEMBERS = URL_LISTS_BASE +"members.json";
const string URL_LISTS_DESTROY = URL_LISTS_BASE +"destroy.json";

const string URL_ACCOUNT_RATE_LIMIT_STATUS = URL_ACCOUNT_BASE +"rate_limit_status.json";
const string URL_ACCOUNT_VERIFY_CREDENTIALS = URL_ACCOUNT_BASE +"verify_credentials.json";
const string URL_ACCOUNT_END_SESSION = URL_ACCOUNT_BASE +"end_session.json";
const string URL_ACCOUNT_UPDATE_PROFILE = URL_ACCOUNT_BASE +"update_profile.json";
const string URL_ACCOUNT_UPDATE_PROFILE_BACKGROUND_IMAGE = URL_ACCOUNT_BASE +"update_profile_background_image.json";
const string URL_ACCOUNT_UPDATE_PROFILE_COLORS = URL_ACCOUNT_BASE +"update_profile_colors.json";
const string URL_ACCOUNT_UPDATE_PROFILE_IMAGE =  URL_ACCOUNT_BASE +"update_profile_image.json";
const string URL_ACCOUNT_TOTALS = URL_ACCOUNT_BASE +"totals.json";
const string URL_ACCOUNT_SETTINGS = URL_ACCOUNT_BASE +"settings.json";

const string URL_AUTHORIZE = "http://twitter.com/oauth/authorize?oauth_token=";


/*
// set keys
	twitter.setTwitterUsername("roxlutest");
	twitter.setTwitterPassword("*************");
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("***************");
	
	string token_file = ofToDataPath("twitter.txt", true);
	//twitter.removeTokens(token_file);
	
	// get authorized tokens.
	if(!twitter.loadTokens(token_file)) {
		string auth_url;
		twitter.requestToken(auth_url);
		twitter.handlePin(auth_url);
		twitter.accessToken();
		twitter.saveTokens(token_file);
	}
	
	twitter.statusesHomeTimeline();
	printf("%s\n", twitter.getResponse().c_str());

*/

class Twitter {
public:
	Twitter();
	~Twitter();

	bool requestToken(string& authURL /* out */);
	bool accessToken();
	bool handlePin(const string& authURL); 
	bool setPin(const string& pin);
	
	void setTwitterUsername(const string& username);
	void setTwitterPassword(const string& password);
	string getTwitterUsername();
	string getTwitterPassword(); 
	
	void setConsumerKey(const string& consumerKey);
	void setConsumerSecret(const string& consumerSecret);
	
	bool saveTokens(const string& filePath);
	bool loadTokens(const string& filePath);
	bool removeTokens(const string& filePath);
	
	// API: Tweets
	bool statusesRetweetedBy(const string& tweetID, rtp::Collection* extraParams =  NULL);
	bool statusesRetweetedByIDs(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesRetweets(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesShow(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesDestroy(const string& tweetID, rtp::Collection* extraParams = NULL); 
	bool statusesRetweet(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool statusesUpdate(const string& tweet, rtp::Collection* extraParams =  NULL);
	bool statusesUpdateWithMedia(const string& tweet, const string& imageFilePath, rtp::Collection* extraParams =  NULL);
	bool statusesoEmbed(const string& tweetID, rtp::Collection* extraParams =  NULL); 
		
	// API: Timelines
	bool statusesHomeTimeline(rtp::Collection* extraParams =  NULL);
	bool statusesMentions(rtp::Collection* extraParams =  NULL);
	bool statusesPublicTimeline(rtp::Collection* extraParams =  NULL);
	bool statusesRetweetedByMe(rtp::Collection* extraParams = NULL);
	bool statusesRetweetedToMe(rtp::Collection* extraParams = NULL);
	bool statusesRetweetsOfMe(rtp::Collection* extraParams = NULL);
	bool statusesUserTimeline(rtp::Collection* extraParams = NULL);
	bool statusesRetweetedToUser(const string& screenName, rtp::Collection* extraParams = NULL);
	bool statusesRetweetedByUser(const string& screenName, rtp::Collection* extraParams = NULL);
	
	// API: search
	bool search(const string& query, rtp::Collection* extraParams = NULL);

	// API: search (your Twitter application needs to have permissions!)
	bool directMessages(rtp::Collection* extraParams = NULL);
	bool directMessagesSent(rtp::Collection* extraParams = NULL);
	bool directMessagesDestroy(const string& messageID, rtp::Collection* extraParams = NULL);
	bool directMessagesNew(const string& screenName, const string& text, rtp::Collection*  extraParams = NULL);
	bool directMessagesShow(const string& messageID);
	
	// API: Friends & followers
	bool followersIDs(const string& screenName, rtp::Collection* extraParams = NULL);
	bool friendsIDs(const string& screenName, rtp::Collection* extraParams = NULL);
	bool friendshipsExists(const string& screenNameA, const string& screenNameB);
	bool friendshipsIncoming(rtp::Collection* extraParams = NULL);
	bool friendshipsOutgoing(rtp::Collection* extraParams = NULL);
	bool friendshipsShow(const string& sourceScreenName, const string& targetScreenName);
	bool friendshipsCreate(const string& screenName, rtp::Collection* extraParams = NULL);
	bool friendshipsDestroy(const string& screenName, rtp::Collection* extraParams = NULL);
	bool friendshipsLookup(const string& screenNames);
	bool friendshipsUpdate(const string& screenName, bool enableDeviceNotifications, bool enableRetweets);
	bool friendshipsNoRetweetIDs(bool stringifyIDs = true);
	
	// API: Users
	bool usersLookup(const string& screenNames, rtp::Collection* extraParams = NULL);
	bool usersSearch(const string& q, rtp::Collection* extraParams = NULL);
	bool usersShow(const string& screenName, rtp::Collection* extraParams = NULL);
	bool usersContributees(const string& screenName, rtp::Collection* extraParams = NULL);
	bool usersContributors(const string& screenName, rtp::Collection* extraParams = NULL);
	//bool usersProfileImages();  // how do we implement this.. maybe parse the 302

	// API: Users suggestions
	bool usersSuggestions();
	bool usersSuggestionsSlug(const string& slug, string lang = "");
	bool usersSuggestionsSlugMembers(const string& slug);
	
	// API: Favorites
	bool favorites(rtp::Collection* extraParams = NULL);
	bool favoritesCreate(const string& tweetID, rtp::Collection* extraParams = NULL);
	bool favoritesDestroy(const string& tweetID);
	
	// API: Lists
	bool listsAll(const string& screenName);
	bool listsStatuses(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams = NULL);		
	bool listsMembersDestroy(const string& slug, const string& ownerScreenName);
	bool listsMemberships(const string& screenName, rtp::Collection* extraParams = NULL);
	bool listsSubscribers(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams = NULL);
	bool listsSubscribersCreate(const string& slug, const string& ownerScreenName); // @todo not working; doc is outdated
	bool listsSubscribersShow(const string& slug, const string& ownerScreenName, const string& screenName, rtp::Collection* extraParams = NULL);
	bool listsSubscribersDestroy(const string& slug, const string& ownerScreenName); // @todo url does not exist??
	bool listsMembersCreateAll(const string& slug, const string& ownerScreenName, const string& screenNames);
	bool listsMembersShow(const string& slug, const string& ownerScreenName, const string& screenName, rtp::Collection* extraParams = NULL);	
	bool listsMembers(const string& slug, const string& ownerScreenName, rtp::Collection* extraParams = NULL);
	bool listsMembersCreate(const string& slug, const string& ownerScreenName, const string& screenName);
	bool listsDestroy(const string& slug, const string& ownerScreenName);
	bool listsUpdate(const string& slug, const string& ownerScreenName, bool isPublic = true, const string& description = "");
	bool listsCreate(const string& name, bool isPublic = true, const string& description = "");
	bool lists(const string& screenName, rtp::Collection* extraParams = NULL);
	bool listsShow(const string& slug, const string& ownerScreenName);
	bool listsSubscriptions(const string& screenName, rtp::Collection* extraParams = NULL);
	
	// API: Accounts
	bool accountRateLimitStatus();
	bool accountVerifyCredentials(rtp::Collection* extraParams = NULL);
	bool accountEndSession();
	bool accountUpdateProfile(const string& name, const string& url, const string& location, const string& description, rtp::Collection* extraParams = NULL);
	bool accountUpdateProfileBackgroundImage(const string& imageFilePath, bool tile, bool use, rtp::Collection* extraParams = NULL);
	bool accountUpdateProfileImage(const string& imageFilePath);
	bool accountUpdateProfileColors(const string& bgColor, const string& profileLinkColor, const string& sidebarBorderColor, const string& sidebarFillColor, const string& textColor, rtp::Collection* extraParams = NULL);
	bool accountTotals();
	bool accountSettings();
	
	// Event system.
	void addEventListener(IEventListener& listener);
	void addEventListener(IEventListener* listener);
	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
	
	// ++
	roxlu::twitter::parser::JSON& getJSON();
	string& getResponse();	
	rto::oAuth& getoAuth();
	
private:
	bool doGet(const string& url, rtp::Collection* defaultParams = NULL, rtp::Collection* extraParams = NULL);
	bool doPost(const string& url, const rtp::Collection* col = NULL, bool multiPart = false, rtp::Collection* extraParams = NULL);
	bool doPost(const string& url, bool multiPart = false, rtp::Collection* extraParams = NULL);
	void reset();
	string response;
	rto::oAuth oauth;
	rtc::Curl twitcurl;
	vector<IEventListener*> listeners;
	roxlu::twitter::parser::JSON json;
};

inline bool Twitter::doGet(const string& url, rtp::Collection* defaultParams, rtp::Collection* extraParams) {
	reset();
	rtc::Request req(url);
	req.isGet(true);
	
	if(defaultParams != NULL) {
		req.getParams() += *defaultParams;
	}
		
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	
	
	oauth.authorize(req);
	return req.doGet(twitcurl, response);
}


inline bool Twitter::doPost(const string& url, bool multiPart, rtp::Collection* extraParams) {
	reset();
	rtc::Request req(url);
	
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response, multiPart);

}

inline bool Twitter::doPost(const string& url, const rtp::Collection* col, bool multiPart, rtp::Collection* extraParams) {	
	reset();
	rtc::Request req(url);
	
	if(col != NULL) {
		req.addParams(*col);
	}
	
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	req.isPost(true);
	oauth.authorize(req);
	return req.doPost(twitcurl, response, multiPart);
}


inline void Twitter::setTwitterUsername(const string& username) {
	twitcurl.setAuthUsername(username);
}

inline void Twitter::setTwitterPassword(const string& password) {
	twitcurl.setAuthPassword(password);
}

inline void Twitter::setConsumerKey(const string& key) {
	oauth.setConsumerKey(key);
}

inline void Twitter::setConsumerSecret(const string& secret) {
	oauth.setConsumerSecret(secret);
}

inline string& Twitter::getResponse() {
	return response;
}

inline rto::oAuth& Twitter::getoAuth() {
	return oauth;
}

inline string Twitter::getTwitterUsername() {
	return twitcurl.getAuthUsername();
}

inline string Twitter::getTwitterPassword() {
	return twitcurl.getAuthPassword();
} 
	
inline roxlu::twitter::parser::JSON& Twitter::getJSON() {
	return json;
}
	
}} // roxlu twitter


namespace rt = roxlu::twitter;

#endif
