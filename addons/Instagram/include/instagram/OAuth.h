#ifndef ROXLU_INSTAGRAM_OAUTH
#define ROXLU_INSTAGRAM_OAUTH

#include <string> 

namespace roxlu {
  class OAuth {
  public:
    OAuth();
    ~OAuth();
    void setup(const std::string consumerKey, std::string consumerSecret);
    void setToken(const std::string token);
    std::string getToken();
  private:
    std::string token;
    std::string consumer_key;
    std::string consumer_secret;
  };

  inline void OAuth::setToken(const std::string t) {
    token = t;
  }

  inline std::string OAuth::getToken() {
    return token;
  }

} // roxlu
#endif
