#include <roxlu/core/Log.h>
#include <twitter/Tweet.h>

//-------------------------------------------------------------------------------

#define TW_PARSE_JSON_STR(json_parent, json_el, json_field, member) \
  json_el = json_object_get(json_parent, json_field);               \
  if(json_is_string(json_el)) {                                     \
    member = json_string_value(json_el);                            \
  }

#define TW_PARSE_JSON_BOOL(json_parent, json_el, json_field, member)  \
  json_el = json_object_get(json_parent, json_field);                 \
  if(json_is_boolean(json_el)) {                                      \
    member = json_is_true(json_el);                                   \
  }

#define TW_PARSE_JSON_INT(json_parent, json_el, json_field, member) \
  json_el = json_object_get(json_parent, json_field);               \
  if(json_is_integer(json_el)) {                                    \
    member = json_integer_value(json_el);                           \
  }

//-------------------------------------------------------------------------------

#define TW_PRINT_BOOL(field) \
  RX_VERBOSE("%s = %s", #field, ((field) ? "true" : "false"));

#define TW_PRINT_INT(field) \
  RX_VERBOSE("%s = %ld", #field, field);

#define TW_PRINT_STR(field) \
  RX_VERBOSE("%s = %s", #field, field.c_str());


//-------------------------------------------------------------------------------

TweetUser::TweetUser() {
  reset(); /* clears all members */
}

void TweetUser::reset() {
  id = 0;
  id_str.clear();
  name.clear();
  screen_name.clear();
  location.clear();
  description.clear();
  url.clear();
  // @todo entities
  is_protected = false;
  followers_count = 0;
  friends_count = 0;
  listed_count = 0;
  created_at.clear();
  favourites_count = 0;
  utc_offset = 0;
  // @todo time_zone
  geo_enabled = false;
  verified = false;
  statuses_count = 0;
  lang.clear();
  contributors_enabled = false;
  is_translator = false;;
  profile_background_color.clear();
  profile_background_image_url.clear();
  profile_background_image_url_https.clear();
  profile_background_tile = false;
  profile_image_url.clear();
  profile_image_url_https.clear();
  profile_link_color.clear();
  profile_sidebar_border_color.clear();
  profile_text_color.clear();
  profile_use_background_image = false;
  default_profile = false;;
  default_profile_image = false;
  following = false;
  follow_request_sent = false;
  notifications = false;
}


Tweet::Tweet() {
  reset(); /* clears all members */
}

bool Tweet::parseJSON(std::string json) {
  json_t* root;
  json_error_t err;

  root = json_loads(json.c_str(), 0, &err);
  if(!root) {
    RX_ERROR("error on line: %d, %s", err.line, err.text);
    return false;
  }

  json_t* el;
  TW_PARSE_JSON_STR(root, el, "created_at", created_at);
  TW_PARSE_JSON_INT(root, el, "id", id);
  TW_PARSE_JSON_STR(root, el, "id_str", id_str);
  TW_PARSE_JSON_STR(root, el, "text", text);
  TW_PARSE_JSON_STR(root, el, "source", source);
  TW_PARSE_JSON_BOOL(root, el, "truncated", truncated);
  TW_PARSE_JSON_INT(root, el, "in_reply_to_status_id", in_reply_to_status_id);
  TW_PARSE_JSON_STR(root, el, "in_reply_to_status_id_str", in_reply_to_status_id_str);
  TW_PARSE_JSON_INT(root, el, "in_reply_to_user_id", in_reply_to_user_id);
  TW_PARSE_JSON_STR(root, el, "in_reply_to_user_id_str", in_reply_to_user_id_str);
  // @todo user
  // @todo geo
  // @todo coordinates
  // @todo place
  // @todo contributors
  TW_PARSE_JSON_INT(root, el, "retweet_count", retweet_count);
  TW_PARSE_JSON_INT(root, el, "favorite_count", favorite_count);
  // @todo entities
  TW_PARSE_JSON_BOOL(root, el, "favorited", favorited);
  TW_PARSE_JSON_BOOL(root, el, "retweeted", retweeted);
  TW_PARSE_JSON_STR(root, el, "lang", lang);

  return true;
}

void Tweet::print() {
  TW_PRINT_STR(created_at);
  TW_PRINT_INT(id);
  TW_PRINT_STR(id_str);
  TW_PRINT_STR(text);
  TW_PRINT_STR(source);
  TW_PRINT_BOOL(truncated);
  TW_PRINT_INT(in_reply_to_status_id);
  TW_PRINT_STR(in_reply_to_status_id_str);
  TW_PRINT_INT(in_reply_to_user_id);
  TW_PRINT_STR(in_reply_to_user_id_str);
  // @todo user
  // @todo geo
  // @todo coordinates
  // @todo place
  // @todo contributors
  TW_PRINT_INT(retweet_count);
  TW_PRINT_INT(favorite_count);
  // @todo entities
  TW_PRINT_BOOL(favorited);
  TW_PRINT_BOOL(retweeted);
  TW_PRINT_STR(lang);
}

void Tweet::reset() {
  created_at.clear();
  id = 0;
  id_str.clear();
  text.clear();
  source.clear();
  truncated = false;;
  in_reply_to_status_id = 0;
  in_reply_to_status_id_str.clear();
  in_reply_to_screen_name.clear();
  in_reply_to_status_id_str.clear();
  in_reply_to_user_id = 0;
  in_reply_to_user_id_str.clear();
  user.reset();
  // @todo geo
  // @todo coordinates
  // @todo place
  // @todo contributors
  retweet_count = 0;
  favorite_count = 0;
  // @todo entities
  favorited = false;
  retweeted = false;;
  lang.clear();
}
