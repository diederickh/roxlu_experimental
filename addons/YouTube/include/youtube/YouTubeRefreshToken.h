/*
  # YouTubeRefreshToken
  
  To perform authorized requests (such as uploading new videos), you must have a valid access token; because
  the access token is only valid for a limited duration we need to refresh it so now and then. This class 
  will refresh the access token using the given refresh token 
 */

#ifndef ROXLU_YOUTUBE_REFRESH_TOKEN_H
#define ROXLU_YOUTUBE_REFRESH_TOKEN_H

#include <youtube/YouTubeModel.h>
#include <youtube/YouTubeTypes.h>
#include <curl/curl.h>
#include <string>
#include <jansson.h>

size_t youtube_refresh_token_data_cb(char* ptr, size_t size, size_t nmemb, void* user);

class YouTubeRefreshToken {
 public:
  YouTubeRefreshToken();
  ~YouTubeRefreshToken();
  bool refresh(std::string clientID, std::string clientSecret, std::string refreshToken);    /* call this to refresh the access token when the timeout has been reached */
  std::string getAccessToken();                                                              /* return the parsed access token; this will only return a valid result when refresh() succeeded */
  int64_t getTokenTimeOut();                                                                 /* returns the parsed token timeout; this will only return a valid result when refresh() succeeded */
 private:
  bool parseJSON();                                                                          /* parses the received JSON; returns true on success; gets called by refresh() */
 public:
  CURL* curl;
  std::string json;                                                                           /* the received data on success */
  std::string access_token;                                                                   /* the received access token */
  int64_t expire_value;                                                                       /* the timeout of the access token */
};

inline std::string YouTubeRefreshToken::getAccessToken() {
  return access_token;
}

inline int64_t YouTubeRefreshToken::getTokenTimeOut() {
  return expire_value;
}


#endif
