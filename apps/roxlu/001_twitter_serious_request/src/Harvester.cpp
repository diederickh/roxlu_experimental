#include <Harvester.h>

Harvester::Harvester() {
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
  tw.apiStatusesFilter(tsf, Harvester::onTweet, this);
}

void Harvester::update() {
  tw.update();
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

  json_value* jv = json_parse(d);
  if(jv->type == json_object) {
    for(int i = 0; i < jv->u.object.length; ++i) {
      if(jv->u.object.values[i].value->type == json_null) {
        continue;
      }
      json_value* v = jv->u.object.values[i].value;
      const char* name = jv->u.object.values[i].name;
      const char* value = jv->u.object.values[i].value->u.string.ptr;
      int len = jv->u.object.values[i].value->u.string.length;
      if(len == 0) {
        continue;
      }

      if(strcmp(name, "text") == 0) {
        bson_append_string(&o, "text", value);
        printf("> %s\n", value);
      }
      else if(strcmp(name, "id_str") == 0) {
        bson_append_string(&o, "id_str", value);
      }
      else if(strcmp(name, "created_at") == 0) {
        bson_append_string(&o, "created_at", value);
      }
      else if(strcmp(name, "in_reply_to_status_id_str") == 0) {
        bson_append_string(&o, "in_reply_to_status_id_str", value);
      }
      else if(strcmp(name, "in_reply_to_user_id_str") == 0) {
        bson_append_string(&o, "in_reply_to_user_id_str", value);
      }
      else if(strcmp(name, "geo") == 0) {
          if(v->type == json_object) {
            json_value* coord = v->u.object.values[0].value;
            if(coord->type == json_array && coord->u.array.length == 2) {
              json_value* lat = coord->u.array.values[0];
              json_value* lng = coord->u.array.values[1];
              if(lat->type == json_double && lng->type == json_double) {
                bson_append_start_array(&o, "geo");
                bson_append_double(&o, "0", lat->u.dbl);
                bson_append_double(&o, "1", lng->u.dbl);
                bson_append_finish_array(&o);
              }
            }
          }
      }
      else if(strcmp(name, "coordinate") == 0) {
        // bson_append_string(&o, "coordinate", value);
      }
      else if(strcmp(name, "retweet_count") == 0) {
        // bson_append_string(&o, "retweet_count", value);
      }
      else if(strcmp(name, "retweeted") == 0) {
        if(jv->u.object.values[i].value->type == json_boolean) {
          // bson_append_bool(&o, "retweeted", jv->u.object.values[i].value->u.boolean);
        }
      }
    }
  }  
  bson_finish(&o);
  mongo_insert(&h->mcon, "tweet.messages", &o, NULL);
  //  bson_print(&o);
  bson_destroy(&o);
}
