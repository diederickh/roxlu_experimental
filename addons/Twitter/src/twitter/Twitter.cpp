#include <twitter/Twitter.h>

// TWITTER
// -------
Twitter::Twitter() 
  :request_token_received_callback(NULL)
  ,request_token_received_callback_data(NULL)
  ,access_token_received_callback(NULL)
  ,access_token_received_callback_data(NULL)
  ,api_callback(NULL)
  ,api_callback_data(NULL)
{
}

Twitter::~Twitter() {
}

void Twitter::setConsumer(const char* k) {
  auth.setConsumerKey(k);
}

void Twitter::setConsumerSecret(const char* s) {
  auth.setConsumerSecret(s);
}

void Twitter::setToken(const char* t) {
  auth.setToken(t);
}

void Twitter::setTokenSecret(const char* s) {
  auth.setTokenSecret(s);
}

bool Twitter::hasConsumerKeyAndSecret() {
  if(!auth.hasConsumerKey() || !auth.hasConsumerSecret()) {
    printf("ERROR: set consumer key and secret first\n");
    return false;
  }
  return true;
}

void Twitter::setAPICallback(cb_api cb, void* apiData) {
  api_callback = cb;
  api_callback_data = apiData;
}

void Twitter::onHTTPRead(HTTPConnection* c, void* ctx) {
}

void Twitter::onHTTPClose(HTTPConnection* c, void* ctx) {
  Twitter* t = static_cast<Twitter*>(ctx);
  
  switch(c->state) {

  case TCS_REQUEST_TOKEN_OPEN: { 
    c->state = TCS_REQUEST_TOKEN_CLOSED;
    t->parseBuffer(c);
    break;
  }

  case TCS_EXCHANGE_REQUEST_TOKEN_OPEN: {
    c->state = TCS_EXCHANGE_REQUEST_TOKEN_CLOSED;
    t->parseBuffer(c);
    break;
  }

  case TCS_API_REQUEST_OPEN: {
    c->state = TCS_API_REQUEST_CLOSED;
    t->parseBuffer(c);
    break;
  };

  };
}

void Twitter::requestToken(cb_request_token_received receivedCB, void* receivedData) { 
  if(!hasConsumerKeyAndSecret()) {
    return;
  }
  request_token_received_callback = receivedCB;
  request_token_received_callback_data = receivedData;

  auth.setCallback("oob");

  HTTPRequest req;
  req.setURL(HTTPURL("http", "api.twitter.com", "/oauth/request_token"));
  req.addHeader(HTTPHeader("User-Agent", "twitter-client"));
  req.setMethod(REQUEST_POST);
  auth.addAuthorizationHeadersToRequest(req);

  HTTPConnection* c = http.sendRequest(req, NULL, NULL, Twitter::onHTTPClose, this);
  c->state = TCS_REQUEST_TOKEN_OPEN;
}

void Twitter::exchangeRequestTokenForAccessToken(
                                                 std::string requestToken
                                                 ,std::string verifier
                                                 ,cb_access_token_received accessCB
                                                 ,void* accessData
                                                 )
{
  if(!hasConsumerKeyAndSecret()) {
    return;
  }
  access_token_received_callback = accessCB;
  access_token_received_callback_data = accessData;
  
  auth.reset();

  HTTPRequest req;
  req.setURL(HTTPURL("http", "api.twitter.com", "/oauth/access_token"));
  req.addContentParameter(HTTPParameter("oauth_verifier", verifier));
  req.setMethod(REQUEST_POST);
  req.addHeader(HTTPHeader("User-Agent", "twitter-client"));

  auth.setToken(requestToken);
  auth.addAuthorizationHeadersToRequest(req);
  HTTPConnection* c = http.sendRequest(req, NULL, NULL, Twitter::onHTTPClose, this);
  c->state = TCS_EXCHANGE_REQUEST_TOKEN_OPEN;
}

void Twitter::makeAPICall(const std::string endpoint, HTTPParameters params) {
  auth.reset();

  HTTPRequest req;
  req.setURL(HTTPURL("http", "api.twitter.com", endpoint));
  req.addHeader(HTTPHeader("User-Agent", "twitter-client"));
  req.copyContentParameters(params);
  req.setMethod(REQUEST_POST);
  auth.addAuthorizationHeadersToRequest(req);
  HTTPConnection* c = http.sendRequest(req, NULL, NULL, Twitter::onHTTPClose, this);
  c->state = TCS_API_REQUEST_OPEN;
}

void Twitter::update() {
  http.update();
}


void Twitter::parseBuffer(HTTPConnection* c) {
  printf("> Twitter::parseBuffer()\n");
  switch(c->state) {

  case TCS_REQUEST_TOKEN_OPEN: {
    return;
  }

  case TCS_REQUEST_TOKEN_CLOSED: {
    std::string body = c->response.getBody(c->buffer);
    HTTPParameters parameters;
    size_t found = parameters.createFromVariableString(body);
    if(found > 0
       && parameters.exist("oauth_token") 
       && parameters.exist("oauth_token_secret") 
       && parameters.exist("oauth_callback_confirmed")) 
      {
        if(request_token_received_callback) {
          std::string redirect_url = "https:://api.twitter.com/oauth/authenticate?oauth_token=" +parameters["oauth_token"].value;
          request_token_received_callback(
                            parameters["oauth_token"].value
                            ,parameters["oauth_token_secret"].value
                            ,redirect_url
                            ,request_token_received_callback_data
                            );
        }
      }
    else {
      printf("ERROR: Unhandled. We did not find any variables in response from twitter..\n");
    }
     break;
  };


  case TCS_EXCHANGE_REQUEST_TOKEN_CLOSED: {
    std::string body = c->response.getBody(c->buffer);
    HTTPParameters p;
    size_t found = p.createFromVariableString(body);
    if(found > 0
       && p.exist("oauth_token")
       && p.exist("oauth_token_secret")
       && p.exist("user_id")
       && p.exist("screen_name"))
      {
        if(access_token_received_callback) {
          TwitterTokenInfo info;
          info.oauth_token = p["oauth_token"].value;
          info.oauth_token_secret = p["oauth_token_secret"].value;
          info.user_id = p["user_id"].value;
          info.screen_name = p["screen_name"].value;
          access_token_received_callback(info, access_token_received_callback_data);
        }
      }
    else {
    }
    break;
  }

  case TCS_API_REQUEST_CLOSED: {
    std::string body = c->response.getBody(c->buffer);
    if(api_callback) {
      api_callback(body, api_callback_data);
    }
    break;
  };

  default:printf("ERROR: Unhandled state parseBuffer(HTTPConnection) .\n"); break;
  }
};


// API IMPLEMENTATION
// -----------------------------------------------------------------
void Twitter::apiStatusesUpdate(const TwitterStatusesUpdate& param) {
  HTTPParameters p;
  p.add("status", param.status);
  makeAPICall("/1.1/statuses/update.json", p);
}
