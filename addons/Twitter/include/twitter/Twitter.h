#ifndef ROXLU_TWITTERH
#define ROXLU_TWITTERH

// AWESOME: http://blog.brookmiles.ca/2010/07/14/twitter-oauth-in-c-for-win32-part-2/
#include <cstdlib>
#include <iostream>
#include <vector>
#include <time.h>

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
}

#include <twitter/HTTP.h>
#include <twitter/Buffer.h>
#include <twitter/OAuth.h>
#include <twitter/Types.h>
#include <twitter/SSLBuffer.h>


namespace roxlu {
  namespace twitter { 
    class Twitter;

    // After exchanging a token request we get this info:
    struct TwitterTokenInfo {
      std::string oauth_token;
      std::string oauth_token_secret;
      std::string user_id;
      std::string screen_name;
    };

    // -----------------------------------
    struct TwitterStatusesUpdate {
      TwitterStatusesUpdate(const std::string status):status(status){}
      std::string status;
    };

    struct TwitterStatusesFilter {
      TwitterStatusesFilter(const std::string& track);
      std::string getCommaSeparatedTrackList() const;
      std::vector<std::string> track;
    };

    struct TwitterCallParams {
      TwitterCallParams():cb(NULL),data(NULL) {}
      std::string endpoint; 
      std::string host;
      HTTPParameters params;
      bool secure;
      cb_request_read_body cb; 
      void* data;
    };
    // -----------------------------------


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
      void requestToken(cb_request_token_received receivedCB, void* receivedData = NULL);
      void exchangeRequestTokenForAccessToken(
                                              std::string requestToken
                                              ,std::string verifier
                                              ,cb_access_token_received accessCB
                                              ,void* data
                                              ); 
      void setSSLPrivateKey(const char* filepath);
      void update();
      void makeAPICall(TwitterCallParams& p);
      void apiStatusesFilter(const TwitterStatusesFilter& param, cb_request_read_body bodyCB = NULL, void* bodyData = NULL);
      void apiStatusesUpdate(const TwitterStatusesUpdate& param, cb_request_read_body bodyCB = NULL, void* bodyData = NULL);
    private:
      bool hasConsumerKeyAndSecret();
      static void requestTokenCallback(const char* data, size_t len, void* twitter);
      static void exchangeRequestTokenForAccessTokenCallback(const char* data, size_t len, void* twitter);
      static int verifySSLCallback(int ok, X509_STORE_CTX* store);
    private:
      cb_request_token_received request_token_received_callback;
      cb_access_token_received access_token_received_callback;
      void* request_token_received_callback_data;
      void* access_token_received_callback_data;
      OAuth auth;
      HTTP http; 
      SSL_CTX* ssl_ctx;
    };
  } // roxlu
} // twitter
#endif
