#ifndef ROXLU_HARVERSTER_TYPESH
#define ROXLU_HARVERSTER_TYPESH

#include <iostream>

struct TweetUser {
  TweetUser()
    :followers_count(0)
    ,geo_enabled(false)
  {
  }

  std::string id_str;
  std::string screen_name;
  std::string name;
  std::string location;
  int followers_count;
  bool geo_enabled;
};

struct TweetMessage {
  std::string text;
  std::string id_str;
  std::string created_at;
  std::string in_reply_to_status_id_str;
  std::string in_reply_to_user_id_str;

  void print() {
    printf("text: %s\n", text.c_str());
    printf("id_str: %s\n", id_str.c_str());
    printf("created_at: %s\n", created_at.c_str());
    printf("in_reply_to_status_id_str: %s\n", in_reply_to_status_id_str.c_str());
    printf("in_reply_to_user_id_str: %s\n", in_reply_to_user_id_str.c_str());
    printf("--\n");
  }
};

#endif
