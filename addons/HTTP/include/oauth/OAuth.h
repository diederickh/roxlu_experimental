#ifndef ROXLU_OAUTH_H
#define ROXLU_OAUTH_H

#include <string>
#include <vector>

#if defined(_WIN32)
#  include <windows.h>
#  include <mmsystem.h>
#  include <sys/timeb.h>
#endif

#include <iterator>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <crypto/Base64Encoder.h>
#include <crypto/HMAC_SHA1.h>
#include <http/HTTPRequest.h>
#include <http/HTTPParameters.h>
#include <http/HTTPParameter.h>
#include <http/HTTPURL.h>

#ifndef u64
#  ifdef _WIN32
      typedef unsigned __int64   u64;
#  else
      typedef unsigned long long u64;
#  endif
#endif

//-------------------------------------------------------------------------------

class OAuth {
 public:
  OAuth();
  ~OAuth();
  void setConsumer(std::string consumerKey, std::string consumerSecret);          /* set the consumer key and secret. The consumer key/secret is used to identify the application (the client) */
  void setToken(std::string token, std::string tokenSecret);                      /* the token represents the authorization information of the user for which the consumer (client) will make requests */
  void addAuthorizationHeadersToRequest(HTTPRequest& r);                          /* calculates the signature with basestring and add the authorization header for oauth to the request so you can make authorized requests */
  void reset();
  std::string getConsumerKey();                                                   /* returns the consumer key you passed into `setConsumer()` */
  std::string getConsumerSecret();                                                /* returns the consumer secret you passed into `setConsumer()` */
 private:
  void updateNonceAndTimestamp();                                                 /* Number used ONCE, this is used by the server to make sure one request can be made only once (when the request gets comprimised it can't be used forever by the other party). This functions update the epoch value + nonce value */
  std::string generateNonce();                                                    /* Generates a nonce value */
  std::string createSignature(HTTPRequest& r);
  std::string createSignatureBaseString(HTTPRequest& r);
  std::string createSignatureSignKey();
  std::string createNormalizedURL(HTTPURL& url);
  HTTPParameters collectParameters(HTTPRequest& r);
  void copyParametersForSignature(HTTPParameters& from, HTTPParameters& into);
  void setParameters();
  void print();

  /* adding parameters to the internallay used parameters container */
  void addConsumerKeyParameter();
  void addNonceParameter();
  void addTimestampParameter();
  void addSignatureMethodParameter();                                             /* add the signature method that we use; for now only HMAC-SHA1 supported (as this is one of the most commonly used methods) */
  void addVersionParameter();
  void addTokenParameter();

 private:
  u64 epoch;
  std::string nonce;                                                              /* the "number used once" value */
  std::string consumer_key;                                                       /* consumer key for the application (consumer is client application which acts upon the user/resource owner) */
  std::string consumer_secret;                                                    /* consumer secret for the application (consumer is client application which acts upon the user/resource owner) */
  std::string token;                                                              /* token represents the user on behalf of whom we make reqeusts */
  std::string token_secret;                                                       /* token secret is used on behalf of the user for who we make requests */
  HTTPParameters auth_params;                                                     /* internally used to construct the authorization header, signature etc.. */
};

inline void OAuth::setConsumer(std::string consumerKey, std::string consumerSecret) {
  consumer_key = consumerKey;
  consumer_secret = consumerSecret;
}

inline void OAuth::setToken(std::string tok, std::string tokSecret) {
  token = tok;
  token_secret = tokSecret;
}

//-------------------------------------------------------------------------------

// epoch timestamp
inline u64 oauth_timestamp() {
#ifndef _WIN32
  u64 t = time(0);
  return t;
#else
  struct timeb t;
  ftime(&t);
  return t.time;
#endif
}


inline std::string OAuth::getConsumerKey() {
  return consumer_key;
}

inline std::string OAuth::getConsumerSecret() {
  return consumer_secret;
}

#endif

