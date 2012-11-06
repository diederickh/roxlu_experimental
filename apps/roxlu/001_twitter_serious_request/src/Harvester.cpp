#include <Harvester.h>


Harvester::Harvester() 
  :num(0)
{
}

Harvester::~Harvester() {
  mongo_destroy(&mcon);
}

void Harvester::setup() {
  mongo_init(&mcon);
  mongo_set_op_timeout(&mcon, 1000);
  int r = mongo_connect(&mcon, "127.0.0.1", 27017);
  if(r != MONGO_OK) {
    printf("ERROR: Cannot connect to mongo.\n");
    ::exit(0);
  }

  // follow these tags (lowercase)
  tags.push_back("3fm");
  tags.push_back("3fmsr");
  tags.push_back("sr12");
  tags.push_back("twitter");
  tags.push_back("love");
  tags.push_back("sex");

  // query tweets
  /*
  mongo_cursor cursor;
  mongo_cursor_init(&cursor, &mcon, "tweet.messages");
  while(mongo_cursor_next(&cursor) == MONGO_OK) {
    bson_print(&(cursor.current));
  }
  mongo_cursor_destroy(&cursor);
  */

  std::string key_file = File::toDataPath("client-key.pem");
  printf("%s\n", key_file.c_str());
  tw.setSSLPrivateKey(key_file.c_str());
  #include "Tokens.h"

  roxlu::twitter::TwitterStatusesFilter tsf("twitter,love,sex");
  //roxlu::twitter::TwitterStatusesFilter tsf("3fm,3fmsr,sr12");
  tw.apiStatusesFilter(tsf, Harvester::onTweet, this);
}

void Harvester::update() {
  tw.update();
  kurl.update();
}

void Harvester::onTweet(const char* data, size_t len, void* harv) {
  Harvester* h = static_cast<Harvester*>(harv);

  // null terminate the data
  char* d = (char*)data;
  d[len] = '\0';

 // Create a new bson obj we store.
  bson o;
  bson_init(&o);
  bson_append_new_oid(&o, "_id");
  bson_append_string(&o, "raw", d);

  // Parse the json
  json_t* root;
  json_error_t error;
  root = json_loads(d, 0, &error);
  if(!root) {
    printf("ERROR: cannot parse json\n");
    return;
  }

  if(!json_is_object(root)) {
    printf("ERROR: cannot find root of tweet.\n");
    return;
  }

  json_t* val;

  // id_str
  val = json_object_get(root, "id_str");
  std::string id_str;
  if(json_is_string(val)) {
    bson_append_string(&o, "id_str", json_string_value(val));
  }
  else {
    printf("HUGE ERROR: CANNOT FIND ID_STR; MUST BE INCORRECT JSON\n");
    return;
  }
  
  // Get images + hashtags
  json_t* ents = json_object_get(root, "entities");
  if(json_is_object(ents)) {
    // hashtags
    json_t* hashtags = json_object_get(ents, "hashtags");
    if(json_is_array(hashtags) && json_array_size(hashtags) > 0) {
      size_t num_tags = json_array_size(hashtags);
      for(int k = 0; k < num_tags; ++k) {
        json_t* tag_obj = json_array_get(hashtags, k);

        if(json_is_object(tag_obj)) {
          json_t* tag_txt = json_object_get(tag_obj, "text");
          if(json_is_string(tag_txt)) {
            std::string tag(json_string_value(tag_txt));
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            std::vector<std::string>::iterator it = std::find(h->tags.begin(), h->tags.end(), tag);
            if(it != h->tags.end()) {
              printf("YES FOUND A TAG! %s\n", tag.c_str());
            }
          } 
        }
      }
    }
    // media
    json_t* medias = json_object_get(ents, "media");
    if(json_is_array(medias)) {
      size_t nels = json_array_size(medias);

      for(int i = 0; i < nels; ++i) {
        json_t* media_obj = json_array_get(medias, i);
        if(!json_is_object(media_obj)) {
          continue;
        }

        // media id str
        json_t* media_id_str_obj = json_object_get(media_obj, "id_str");
        std::string media_id_str;
        if(json_is_string(media_id_str_obj)) {
          media_id_str = json_string_value(media_id_str_obj);
        }

        json_t* media_url = json_object_get(media_obj, "media_url");
        if(json_is_string(media_url)) {
          const char* url = json_string_value(media_url);
          http_parser_url u;
          int r = http_parser_parse_url(url, strlen(url)+1, 0, &u);

          char filename[512];
          memcpy(filename, url+u.field_data[UF_PATH].off+1, u.field_data[UF_PATH].len);
          //printf(">>>>>>>>>> (%d), %s <%s>\n",r, json_string_value(media_url), filename);


          // Download the media file
          /*
          HarvestEntry* he = new HarvestEntry();
          h->entries.push_back(he);
          he->type = HT_TWEET;
          he->media_id_str = media_id_str;
          he->h = h;
          h->entries.push_back(he);
          h->kurl.download(url, filename, Harvester::onTweetImageDownloadComplete, he);
          */
        } 

      }
    }
    
  }

  
  // text
  val = json_object_get(root, "text");
  if(json_is_string(val)) {
    bson_append_string(&o, "text", json_string_value(val));
    printf("> %05d =  %s\n", h->num, json_string_value(val));
    ++h->num;
  }
  
  // created_at
  val = json_object_get(root, "created_at");
  if(json_is_string(val)) {
    bson_append_string(&o, "created_at", json_string_value(val));
  }

  // in_reply_to_status_id_str
  val = json_object_get(root, "in_reply_to_status_id_str");
  if(json_is_string(val)) {
    bson_append_string(&o, "in_reply_to_status_id_str", json_string_value(val));
  }

  // in_reply_to_user_id_str
  val = json_object_get(root, "in_reply_to_user_id_str");
  if(json_is_string(val)) {
    bson_append_string(&o, "in_reply_to_user_id_str", json_string_value(val));
  }

  // user
  val = json_object_get(root, "user");
  if(json_is_object(val)) {

    // id_str
    json_t* subval = json_object_get(val, "id_str");
    if(json_is_string(subval)) {
      bson_append_string(&o, "user_id_str", json_string_value(subval));
    }

    // screen_name
    subval = json_object_get(val, "screen_name");
    if(json_is_string(subval)) {
      bson_append_string(&o, "user_screen_name", json_string_value(subval));
    }

    // location
    subval = json_object_get(val, "location");
    if(json_is_string(subval)) {
      bson_append_string(&o, "user_location", json_string_value(subval));
    }

    // lang
    subval = json_object_get(val, "lang");
    if(json_is_string(subval)) {
      bson_append_string(&o, "user_lang", json_string_value(subval));
    }

    // statuses_count
    subval = json_object_get(val, "statuses_count");
    if(json_is_integer(subval)) {
      bson_append_int(&o, "user_statuses_count", json_integer_value(subval));
    }

    // friends count
    subval = json_object_get(val, "friends_count");
    if(json_is_integer(subval)) {
      bson_append_int(&o, "user_friends_count", json_integer_value(subval));
    }

    // followers count
    subval = json_object_get(val, "followers_count");
    if(json_is_integer(subval)) {
      bson_append_int(&o, "user_followers_count", json_integer_value(subval));
    }
  }

  // retweeted
  val = json_object_get(root, "retweeted");
  if(json_is_boolean(val)) {
    bson_append_bool(&o, "", json_is_true(val));
  }

  // https://groups.google.com/forum/?fromgroups=#!topic/twitter-api-announce/5_wG5KjLcVA
  // - geo will be deprecated 
  // - coordinates: new, [long,lat]
  // - place 
  val = json_object_get(root, "coordinates");
  if(json_is_object(val)) {
    json_t* subval = json_object_get(val, "coordinates");
    if(json_is_array(subval) && json_array_size(subval) == 2) {
      double longitude = json_number_value(json_array_get(subval, 0));
      double latitude = json_number_value(json_array_get(subval, 1));
      bson_append_double(&o, "coordinates_long", longitude);
      bson_append_double(&o, "coordinates_lat", latitude);
    }
  }

  bson_finish(&o);
  mongo_insert(&h->mcon, "tweet.messages", &o, NULL);
  //  bson_print(&o);
  bson_destroy(&o);

  return;
}

void Harvester::onTweetImageDownloadComplete(KurlConnection* c, void* userdata) {
  HarvestEntry* he = static_cast<HarvestEntry*>(userdata);
  printf("Harvester complete!: %s\n", he->media_id_str.c_str());
  std::vector<HarvestEntry*>::iterator it = std::find(he->h->entries.begin(), he->h->entries.end(), he);
  if(it != he->h->entries.end()) {
    delete he;
    he->h->entries.erase(it);
  }
}
