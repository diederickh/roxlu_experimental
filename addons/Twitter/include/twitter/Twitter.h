#ifndef ROXLU_TWITTERH
#define ROXLU_TWITTERH

// AWESOME: http://blog.brookmiles.ca/2010/07/14/twitter-oauth-in-c-for-win32-part-2/
#include <cstdlib>
#include <iostream>
#include <vector>
#include <time.h>

extern "C" {
#include <event2/dns.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
}

#include <twitter/HTTP.h>
#include <twitter/Buffer.h>
#include <twitter/OAuth.h>
#include <twitter/Types.h>

class Twitter;

// TYPES
// ------
enum Twittertates {
  TCS_NONE
  ,TCS_REQUEST_TOKEN_OPEN
  ,TCS_REQUEST_TOKEN_CLOSED
  ,TCS_EXCHANGE_REQUEST_TOKEN_OPEN
  ,TCS_EXCHANGE_REQUEST_TOKEN_CLOSED
  ,TCS_API_REQUEST_OPEN
  ,TCS_API_REQUEST_CLOSED
};

// After exchanging a token request we get this info:
struct TwitterTokenInfo {
  std::string oauth_token;
  std::string oauth_token_secret;
  std::string user_id;
  std::string screen_name;
};

struct TwitterStatusesUpdate {
  TwitterStatusesUpdate(const std::string status):status(status){}
  std::string status;
};

// TWITTER CALLBACKS
typedef void(*cb_request_token_received)(std::string token, std::string tokenSecret, std::string redirectURL, void* data);
typedef void(*cb_access_token_received)(TwitterTokenInfo info, void* data);
typedef void(*cb_api)(std::string response, void* data);

// TWITTER
// --------
class Twitter {
public:
  Twitter();
  ~Twitter();
  void setConsumer(const char* consumerKey);
  void setConsumerSecret(const char* consumerSecret);
  void setToken(const char* token);
  void setTokenSecret(const char* tokenSecret);
  void requestToken(cb_request_token_received receivedCB, void* receivedData);
  void exchangeRequestTokenForAccessToken(
                                          std::string requestToken
                                          ,std::string verifier
                                          ,cb_access_token_received accessCB
                                          ,void* data
                                          ); 
  void update();
  void makeAPICall(const std::string endpoint, HTTPParameters params);
  void apiStatusesUpdate(const TwitterStatusesUpdate& param);
  void setAPICallback(cb_api cb, void* data);
  static void onHTTPRead(HTTPConnection* c, void* ctx);
  static void onHTTPClose(HTTPConnection* c, void* ctx);
  static void callbackRead(bufferevent* bev, void* ctx);
  static void callbackEvent(bufferevent* bev, short events, void* ctx);
private:
  void parseBuffer(HTTPConnection* c);
  bool hasConsumerKeyAndSecret();
private:
  cb_request_token_received request_token_received_callback;
  cb_access_token_received access_token_received_callback;
  cb_api api_callback;
  void* request_token_received_callback_data;
  void* access_token_received_callback_data;
  void* api_callback_data;
  OAuth auth;
  HTTP http; 
};

#endif
