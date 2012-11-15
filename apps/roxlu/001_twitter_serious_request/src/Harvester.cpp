#include <Harvester.h>

void HarvesterThread::setup() {
  thread.create(*this);
}

void HarvesterThread::run() {
  printf("START THE THREAD\n");
  harvester.setup();
  printf("--------------------\n");
  while(true) {
    harvester.update();
  }
}

bool HarvesterThread::getSliceCounts(uint64_t& numTweets, uint64_t& numImages) {
  mutex.lock();
  bool r = harvester.isSliceFinished();
  if(r) {
    numTweets = harvester.slice_tweet_count;
    numImages = harvester.slice_image_count;
  }
  mutex.unlock();
  return r;
}

void HarvesterThread::resetSliceCounts() {
  mutex.lock();
  harvester.resetSliceCounts();
  mutex.unlock();
}

// --------------------
HarvestEntry::HarvestEntry() {
  h = NULL;
}

HarvestEntry::~HarvestEntry() {
  //printf("~HarvestEntry()\n");
}

// -------------------
// slice: a time slice in which we count number of tweets.
Harvester::Harvester() 
  :num(0)
  ,slice_tweet_count(0) // how many tweets did we receive during the last slice
  ,slice_image_count(0) // how many images did we receive during the last slice
  ,slice_ends_on(0) // next time we reset the slice count
  ,slice_duration(1000) // measure every X-millis
{
}

Harvester::~Harvester() {
}

void Harvester::setup() {
  if(!db.open("tweets.db")) {
    printf("ERROR: cannot open database.\n");
    ::exit(0);
  }
  bool r = db.query("CREATE TABLE IF NOT EXISTS tweets ("  \
           "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
           "id_str TEXT, " \
           "text TEXT, " \
           "tags TEXT, " \
           "created_at TEXT, " \
           "in_reply_to_status_id_str TEXT, "  \
           "in_reply_to_user_id_str TEXT, " \
           "user_id_str TEXT, " \
           "user_screen_name TEXT, " \
           "user_location TEXT, " \
           "user_lang TEXT, " \
           "user_statuses_count INTEGER, " \
           "user_friends_count INTEGER, " \
           "user_followers_count INTEGER, " \
           "retweeted INTEGER, " \
           "coordinates_long REAL, " \
           "coordinates_lat REAL, " \
           "raw TEXT " \
           ");");
  if(!r) {
    printf("ERROR: cannot create tweets table\n");
    ::exit(0);
  }

  r = db.query("CREATE TABLE IF NOT EXISTS images (" \
           "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
           "tweet_id_str TEXT, " \
           "media_id_str TEXT, " \
           "filepath TEXT " \
           ");");

  if(!r) {
    printf("ERROR: Cannot open images table.\n");
    ::exit(0);
  }

  // follow these tags (lowercase)
  tags.push_back("3fm");
  tags.push_back("3fmsr");
  tags.push_back("sr12");
  //tags.push_back("twitpic");
  //tags.push_back("love");
  //tags.push_back("sex");

  std::string key_file = File::toDataPath("client-key.pem");
  tw.setSSLPrivateKey(key_file.c_str());
  #include "Tokens.h"

  roxlu::twitter::TwitterStatusesFilter tsf;
  tsf.addLocation("3.2976616","50.750449","7.2276122","53.5757042");
  tsf.track(rx_join(tags, ","));
  tw.apiStatusesFilter(tsf, Harvester::onTweet, (void*)this);
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
  if(!json_is_string(val)) {
    return;
  }

  QueryInsert insert = h->db.insert("tweets");
  id_str = json_string_value(val);
  insert.use("id_str", id_str)
    .use("raw",d);
  
  // Get images + hashtags
  json_t* ents = json_object_get(root, "entities");
  if(json_is_object(ents)) {
    // hashtags
    json_t* hashtags = json_object_get(ents, "hashtags");
    if(json_is_array(hashtags) && json_array_size(hashtags) > 0) {
      size_t num_tags = json_array_size(hashtags);
      std::vector<std::string> found_tags;
      for(int k = 0; k < num_tags; ++k) {
        json_t* tag_obj = json_array_get(hashtags, k);

        if(json_is_object(tag_obj)) {
          json_t* tag_txt = json_object_get(tag_obj, "text");
          if(json_is_string(tag_txt)) {
            std::string tag(json_string_value(tag_txt));
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            std::vector<std::string>::iterator it = std::find(h->tags.begin(), h->tags.end(), tag);
            if(it != h->tags.end()) {
              found_tags.push_back(tag);
            }
          } 
        }
      }
      if(found_tags.size()) {
        insert.use("tags", rx_join(found_tags,","));
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
          URLInfo u;

          if(parseURL(url, u)) {
            std::string grid_filename = media_id_str +"." +u.file_ext;

            // Download the media file
            HarvestEntry* he = new HarvestEntry();
            h->entries.push_back(he);
            he->type = HT_TWEET;
            he->media_id_str = media_id_str;
            he->id_str = id_str;
            he->h = h;

            h->entries.push_back(he);
            std::string save_subpath = rx_strftime("%Y/%m/%d/%H/");
            File::createPath(File::toDataPath(save_subpath));
            save_subpath += grid_filename;
            
            h->db.insert("images")
              .use("tweet_id_str", id_str)
              .use("media_id_str", media_id_str)
              .use("filepath", save_subpath)
              .execute();

            printf("\n\n>> IMAGE: %s (%s)\n\n", url, grid_filename.c_str());
            h->kurl.download(url, File::toDataPath(save_subpath).c_str(), Harvester::onTweetImageDownloadComplete, he);
          }
          else {
          }
        } 
      }
    }
  }

  
  // text
  val = json_object_get(root, "text");
  if(json_is_string(val)) {
    printf("> %05lld =  %s\n", h->num, json_string_value(val));
    insert.use("text", json_string_value(val));
    ++h->num;
  }
  
  // created_at
  val = json_object_get(root, "created_at");
  if(json_is_string(val)) {
    insert.use("created_at", json_string_value(val));
  }

  // in_reply_to_status_id_str
  val = json_object_get(root, "in_reply_to_status_id_str");
  if(json_is_string(val)) {
    insert.use("in_reply_to_status_id_str", json_string_value(val));
  }

  // in_reply_to_user_id_str
  val = json_object_get(root, "in_reply_to_user_id_str");
  if(json_is_string(val)) {
    insert.use("in_reply_to_user_id_str", json_string_value(val));
  }

  // user
  val = json_object_get(root, "user");
  if(json_is_object(val)) {

    // id_str
    json_t* subval = json_object_get(val, "id_str");
    if(json_is_string(subval)) {
      insert.use("user_id_str", json_string_value(subval));
    }

    // screen_name
    subval = json_object_get(val, "screen_name");
    if(json_is_string(subval)) {
      insert.use("user_screen_name", json_string_value(subval));
    }

    // location
    subval = json_object_get(val, "location");
    if(json_is_string(subval)) {
      insert.use("user_location", json_string_value(subval));
    }

    // lang
    subval = json_object_get(val, "lang");
    if(json_is_string(subval)) {
      insert.use("user_lang", json_string_value(subval));
    }

    // statuses_count
    subval = json_object_get(val, "statuses_count");
    if(json_is_integer(subval)) {
      insert.use("user_statuses_count", json_integer_value(subval));
    }

    // friends count
    subval = json_object_get(val, "friends_count");
    if(json_is_integer(subval)) {
      insert.use("user_friends_count", json_integer_value(subval));
    }

    // followers count
    subval = json_object_get(val, "followers_count");
    if(json_is_integer(subval)) {
      insert.use("user_followers_count", json_integer_value(subval));
    }
  }

  // retweeted
  val = json_object_get(root, "retweeted");
  if(json_is_boolean(val)) {
    insert.use("retweeted", json_is_true(val) ? 1 : 0);
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
      insert.use("coordinates_long", longitude);
      insert.use("coordinates_lat", latitude);
    }
  }
  
  insert.execute();
  h->slice_tweet_count++;
  json_decref(root);
  return;
}

void Harvester::onTweetImageDownloadComplete(KurlConnection* c, void* userdata) {
  HarvestEntry* he = static_cast<HarvestEntry*>(userdata);
  std::vector<HarvestEntry*>::iterator it = std::find(he->h->entries.begin(), he->h->entries.end(), he);
  if(it != he->h->entries.end()) {
    delete he;
    he->h->entries.erase(it);
  }
}

void Harvester::onTweetImageWriteChunk(
                                       KurlConnection* c,
                                       char* data,
                                       size_t count,
                                       size_t nmemb,
                                       void* userdata
                                       )
{
}
                                       
void Harvester::resetSliceCounts() {
  slice_tweet_count = 0;
  slice_image_count = 0;
  slice_ends_on = Timer::now() + slice_duration;
}

bool Harvester::isSliceFinished() {
  return slice_ends_on < Timer::now();
}


