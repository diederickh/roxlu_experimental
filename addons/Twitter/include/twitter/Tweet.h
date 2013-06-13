#ifndef ROXLU_TWEET_H
#define ROXLU_TWEET_H

#include <string>
#include <jansson.h>

#ifndef u64
#  ifdef _WIN32
typedef unsigned __int64   u64;
#  else
typedef unsigned long long u64;
#  endif
#endif

struct TweetUser {
public:
  TweetUser();
  void reset();
public:
  u64 id;
  std::string id_str;
  std::string name;
  std::string screen_name;
  std::string location;
  std::string description;
  std::string url;
  // @todo entities
  bool is_protected;
  int followers_count;
  int friends_count;
  int listed_count;
  std::string created_at;
  int favourites_count;
  int utc_offset;
  // @todo time_zone
  bool geo_enabled;
  bool verified;
  int statuses_count;
  std::string lang;
  bool contributors_enabled;
  bool is_translator;
  std::string profile_background_color;
  std::string profile_background_image_url;
  std::string profile_background_image_url_https;
  bool profile_background_tile;
  std::string profile_image_url;
  std::string profile_image_url_https;
  std::string profile_link_color;
  std::string profile_sidebar_border_color;
  std::string profile_text_color;
  bool profile_use_background_image;
  bool default_profile;
  bool default_profile_image;
  bool following;
  bool follow_request_sent;
  bool notifications;
};

struct Tweet {
public:
  Tweet();
  void reset();
  bool parseJSON(std::string json);
  void print();

public:
  std::string created_at;
  u64 id;
  std::string id_str;
  std::string text;
  std::string source;
  bool truncated;
  u64 in_reply_to_status_id;
  std::string in_reply_to_status_id_str;
  u64 in_reply_to_user_id;
  std::string in_reply_to_user_id_str;
  std::string in_reply_to_screen_name;
  TweetUser user;
  // @todo geo
  // @todo coordinates
  // @todo place
  // @todo contributors
  int retweet_count;
  int favorite_count;
  // @todo entities
  bool favorited;
  bool retweeted;
  std::string lang;
};

#endif
