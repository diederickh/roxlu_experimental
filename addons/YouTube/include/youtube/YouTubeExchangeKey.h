/*

  # YouTubeExchangeKey

  When you got the auth-code (by opening the html/index.html in a browser and pasting your application
  id in the input field, then pressing GO), then the YouTube class uses this YouTubeExchangeKey class to
  exchange the auth code for a valid access token that should be used to perform authorized requests to the
  youtube api

 */
#ifndef ROXLU_YOUTUBE_EXCHANGE_KEY_H
#define ROXLU_YOUTUBE_EXCHANGE_KEY_H

extern "C" {
#  include <uv.h>
}

#include <string>
#include <youtube/YouTubeTypes.h>
#include <curl/curl.h>
#include <jansson.h>

size_t youtube_exchange_key_data_cb(char* ptr, size_t size, size_t nmemb, void* user);

class YouTubeExchangeKey {
 public:
  YouTubeExchangeKey();
  ~YouTubeExchangeKey();
  bool exchange(std::string code, std::string clientID, std::string clientSecret);
  std::string getRefreshToken();
  std::string getAccessToken();
  int64_t getTokenTimeOut();
 private:
  bool parseJSON();
 public:
  CURL* curl;
  std::string json;
  std::string access_token;
  std::string refresh_token;
  int64_t expire_value;
};

inline std::string YouTubeExchangeKey::getRefreshToken() {
  return refresh_token;
}

inline std::string YouTubeExchangeKey::getAccessToken() {
  return access_token;
}

inline int64_t YouTubeExchangeKey::getTokenTimeOut() {
  return expire_value;
}

#endif
