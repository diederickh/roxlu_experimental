#include <Harvester.h>

void HarvesterThread::setup() {
  thread.create(*this);
}

void HarvesterThread::run() {
  printf("START THE THREAD\n");
  harvester.setup();
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
  mongo_destroy(&mcon);
}

void Harvester::setup() {
  /*
    // Test with parsing a url
  char* test_url = "http://p.twimg.com/A7BYjQsCUAAdYc6.jpg";
  URLInfo u;
  bool parse_result = parseURL(test_url, u);
  printf("proto: %s\n", u.proto.c_str());
  printf("host: %s\n", u.host.c_str());
  printf("filename: %s\n", u.filename.c_str());
  printf("file_ext: %s\n", u.file_ext.c_str());
  printf("parse result: %d\n", parse_result);
  return;
  */

  // setup mongo connections
  mongo_init(&mcon);
  mongo_set_op_timeout(&mcon, 1000);
  int r = mongo_connect(&mcon, "127.0.0.1", 27017);
  if(r != MONGO_OK) {
    printf("ERROR: Cannot connect to mongo.\n");
    ::exit(0);
  }

  // create filesystem
  r = gridfs_init(&mcon, "tweet", "images", gfs);
  if(r != MONGO_OK) {
    printf("ERROR: Cannot initialize file system.\n");
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

  roxlu::twitter::TwitterStatusesFilter tsf;
  tsf.addLocation("3.2976616","50.750449","7.2276122","53.5757042");
  //roxlu::twitter::TwitterStatusesFilter tsf("3fm,3fmsr,sr12");
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
    id_str = json_string_value(val);
  }
  else {
    printf("ERROR: CANNOT FIND ID_STR; MUST BE INCORRECT JSON\n");
    printf("\n\n%s\n\n", d);
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
          URLInfo u;

          if(parseURL(url, u)) {
            std::string grid_filename = u.filename +"." +u.file_ext;;
            std::string grid_mime = "unknown";
            if(u.file_ext == "jpg") {
              grid_mime = "image/jpeg";
            }
            else if(u.file_ext == "png") {
              grid_mime = "image/png";
            }

            // Download the media file
            HarvestEntry* he = new HarvestEntry();
            h->entries.push_back(he);
            he->type = HT_TWEET;
            he->media_id_str = media_id_str;
            he->id_str = id_str;
            he->h = h;
            gridfile_writer_init(he->gfile, h->gfs, grid_filename.c_str(), grid_mime.c_str());
            h->entries.push_back(he);

            printf("gridfile ID: ");
            for(int i = 0; i < sizeof(he->gfile->id.bytes); ++i) {
              printf("%02X", (unsigned char)he->gfile->id.bytes[i]);
            }
            printf("\n");
            // bson_oid_t
            //h->kurl.download(url, filename, Harvester::onTweetImageDownloadComplete, he);
            h->kurl.download(
                             url, 
                             grid_filename.c_str(), 
                             Harvester::onTweetImageDownloadComplete, 
                             he, 
                             Harvester::onTweetImageWriteChunk, 
                             he
                             );
            //  gridfile_writer_done(gfile);
            // gridfile_write_buffer(gfile, buf, bytes_read);

          }
          else {
            printf("\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nCannot parse: %s\n!!!!!!!!!!!!!!!!!!!!!!\n\n", url);
          }

          /* 
             // ue http_parser_url to get pahtname
          http_parser_url u;
          int r = http_parser_parse_url(url, strlen(url)+1, 0, &u);
          char filename[512];
          memcpy(filename, url+u.field_data[UF_PATH].off+1, u.field_data[UF_PATH].len);
          */
          //printf(">>>>>>>>>> (%d), %s <%s>\n",r, json_string_value(media_url), filename);

        } 
      }
    }
  }

  
  // text
  val = json_object_get(root, "text");
  if(json_is_string(val)) {
    bson_append_string(&o, "text", json_string_value(val));
    printf("> %05lld =  %s\n", h->num, json_string_value(val));
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

  h->slice_tweet_count++;
  return;
}

void Harvester::onTweetImageDownloadComplete(KurlConnection* c, void* userdata) {
  HarvestEntry* he = static_cast<HarvestEntry*>(userdata);

  he->h->slice_image_count++;

  std::vector<HarvestEntry*>::iterator it = std::find(he->h->entries.begin(), he->h->entries.end(), he);
  if(it != he->h->entries.end()) {
    int r = gridfile_writer_done(he->gfile);
    if(r != MONGO_OK) {
      printf("ERROR: Cannot close writer.\n");
    }
    
    // STORE TWEET ID IN FILES COLLECTION
    // -----------------------------------
    bson gf_query[1];
    mongo_cursor gf_cursor[1];
    bson_init(gf_query);
    bson_append_oid(gf_query, "_id", &he->gfile->id);
    bson_finish(gf_query);

    bson gf_meta[1];
    bson_init(gf_meta);
    bson_append_start_object(gf_meta, "$set");
    bson_append_string(gf_meta, "tweet_id_str", he->id_str.c_str());
    bson_append_finish_object(gf_meta);
    bson_finish(gf_meta);

    int up_result = mongo_update(&he->h->mcon, "tweet.images.files", gf_query, gf_meta, MONGO_UPDATE_UPSERT, NULL);
    if(up_result != MONGO_OK) {
      printf("ERROR: %s\n", he->h->mcon.errstr);
    }
           
    bson_destroy(gf_meta);
    bson_destroy(gf_query);

    // CLEANUP
    // -------
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
  //printf("Count: %zu, num_bytes: %zu\n", count, nmemb);
  HarvestEntry* he = static_cast<HarvestEntry*>(userdata);
  gridfile_write_buffer(he->gfile, data, count * nmemb);
}
                                       
void Harvester::resetSliceCounts() {
  slice_tweet_count = 0;
  slice_image_count = 0;
  slice_ends_on = Timer::now() + slice_duration;
}

bool Harvester::isSliceFinished() {
  return slice_ends_on < Timer::now();
}


