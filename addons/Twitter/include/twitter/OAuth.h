#ifndef ROXLU_OAUTH
#define ROXLU_OAUTH

#include <iostream>
#include <string>
#include <formats/Base64Encoder.h>
#include <formats/hmac_sha1.h>
#include <twitter/HTTP.h>
#include <twitter/PercentEncode.h>
#include <twitter/Types.h>

namespace roxlu {
  namespace twitter {
    class OAuth {
    public:
      OAuth();
      ~OAuth();
      bool hasConsumerKey();
      bool hasConsumerSecret();
      void setConsumerKey(const std::string key);
      void setConsumerSecret(const std::string sec);
      void setToken(const std::string sec);
      void setTokenSecret(const std::string sec);
      void setCallback(const std::string cb);
      void addAuthorizationHeadersToRequest(HTTPRequest& r);
      void reset();
      void print();
      void updateNonce();
    private:
      void setParameters();
      std::string createSignature(HTTPRequest& r);
      std::string createSignatureBaseString(HTTPRequest& r);
      std::string createSignatureSignKey();
      std::string makeNonce();
      HTTPParameters collectParameters(HTTPRequest& r);
      void addNonceParameter();
      void addTimestampParameter();
      void addSignatureMethodParameter();
      void addVersionParameter();
      void addConsumerKeyParameter();
      void addTokenParameter();
      void addCallbackParameter();
    private:
      u64 epoch;
      std::string callback;
      std::string nonce;
      std::string consumer_key;
      std::string consumer_secret; // used for: sign key 
      std::string token;
      std::string token_secret; // used for: sign key
      HTTPParameters auth_params;
    };

    inline bool OAuth::hasConsumerKey() {
      return consumer_key.size();
    }

    inline bool OAuth::hasConsumerSecret() {
      return consumer_secret.size();
    }

    inline void OAuth::setConsumerKey(const std::string key) {
      consumer_key = key;
    }

    inline void OAuth::setConsumerSecret(const std::string sec) {
      consumer_secret = sec;
    }

    inline void OAuth::setCallback(const std::string cb) {
      callback = cb;
    }

    inline void OAuth::setToken(const std::string sec) {
      token = sec;
    }

    inline void OAuth::setTokenSecret(const std::string sec) {
      token_secret = sec;
    }
  } // roxlu
} // twitter
#endif
