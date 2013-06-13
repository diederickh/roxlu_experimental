#include <twitter/Twitter.h>

//-------------------------------------------------------------------------------
Twitter::Twitter() {
}

Twitter::~Twitter() {
}

// This must be called before making any of the other calls!
bool Twitter::setup(std::string token, std::string tokenSecret, 
                    std::string consumer, std::string consumerSecret)
{
  oauth.setConsumer(consumer, consumerSecret);
  oauth.setToken(token, tokenSecret);
  return true;
}

// Call this as often as possible
void Twitter::update() {
  http.update();
}

// API IMPLEMENTATION
//-------------------------------------------------------------------------------

// https://dev.twitter.com/docs/api/1.1/post/statuses/filter
void Twitter::apiStatusesFilter(TwitterStatusesFilter& param, httpconnection_event_callback cbEvent, void* user) {
  param.setIsSecure(true);
  param.setEndPoint("/1.1/statuses/filter.json");
  param.setHost("stream.twitter.com");
  param.setMethod(HTTP_METHOD_POST);
  param.setCallback(cbEvent, user);
  RX_VERBOSE("%s", param.query_parameters.getQueryString().c_str());
  makeAPICall(param);
}

// https://dev.twitter.com/docs/api/1.1/post/statuses/update
void Twitter::apiStatusesUpdate(TwitterStatusesUpdate& param, httpconnection_event_callback cbEvent, void* user) {
  setDefaultParams(param);
  param.setEndPoint("/1.1/statuses/update.json");
  makeAPICall(param);
}

// https://dev.twitter.com/docs/api/1.1/post/statuses/update_with_media
void Twitter::apiStatusesUpdateWithMedia(TwitterStatusesUpdate& param, httpconnection_event_callback cbEvent, void* user) {
  param.setEndPoint("/1/statuses/update_with_media.json");
  param.setIsSecure(true);
  param.setMethod(HTTP_METHOD_POST);
  param.setHost("upload.twitter.com");
  makeAPICall(param);
}

//-------------------------------------------------------------------------------

// Sets the default of the api call params, which are:
// is_secure = true
// method = HTTP_METHOD_POST
// host = "api.twitter.com"
void Twitter::setDefaultParams(TwitterCallParams& p) {
  p.setIsSecure(true);
  p.setMethod(HTTP_METHOD_POST);
  p.setHost("api.twitter.com");
}

// This will construct a new request and makes sure the correct parameters added.
void Twitter::makeAPICall(TwitterCallParams& p) {
  HTTPProto http_proto = (p.is_secure) ? HTTP_PROTO_HTTPS : HTTP_PROTO_HTTP;

  HTTPURL url(p.host, p.endpoint, http_proto);
  url.addQueryParameters(p.query_parameters);

  HTTPRequest req(url, p.method, HTTP_VERSION_1_1);
  
  req.addContentParameters(p.content_parameters);
  req.addHeader(HTTPHeader("Accept", "*/*"));
  req.addHeader(HTTPHeader("User-Agent", "roxlu-twitter-client/0.1"));
  
  oauth.reset();
  oauth.addAuthorizationHeadersToRequest(req);

  /*
  TwitterCallbackReq* call_req = new TwitterCallbackReq();
  call_req->cb_user = p.cb_user;
  call_req->cb_response = p.cb_response;
  */

  if(p.is_secure) {
    http.sendRequest(req, p.cb_response, p.cb_user, ssl_ctx.allocateSSL());
  }
  else {
    http.sendRequest(req, p.cb_response, p.cb_user);
  }
}
