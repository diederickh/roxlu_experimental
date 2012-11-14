#ifndef ROXLU_HARVESTERH
#define ROXLU_HARVESTERH

#include <vector>
#include <algorithm>
#include <roxlu/Roxlu.h>
#include <twitter/Twitter.h>
#include <kurl/Kurl.h>
#include <Types.h>
#include <jansson.h>
#include <http_parser/http_parser.h>
#include <URLParser.h>

extern "C" {
#include <mongo.h>
#include <gridfs.h>
#include <bson.h>
}

/*
- handy mongodb commands:
  - db.images.files.find( { "_id" : ObjectId("50A162D07AA35C1300000271") } ).toArray()
  - show dbs
  - show collections
  - use [dbname]
  - Get all files which have a tweet_id_str field:
    db.images.files.find( { tweet_id_str : { $exists : true } } ).count()
  - Remove everything from a collection:
     db.images.chunks.remove()
 
 */

// Harvesting real time data from
// - twitter
enum HarvestTypes {
  HT_TWEET
};


class Harvester;
struct HarvestEntry {
  HarvestEntry();
  ~HarvestEntry();

  std::string id_str;
  std::string media_id_str;
  int type;
  Harvester* h;
  gridfile gfile[1];
};

class Harvester {
public:
  Harvester();
  ~Harvester();
  void setup();
  void update();
  void resetSliceCounts();
  bool isSliceFinished();

private:
  static void onTweet(const char* data, size_t len, void* harv);
  static void onTweetImageDownloadComplete(KurlConnection* c, void* harv);
  static void onTweetImageWriteChunk(KurlConnection* c, char* data, size_t count, size_t nmemb, void* userdata);
public:
  roxlu::twitter::Twitter tw;
  mongo mcon;
  Kurl kurl;
  http_parser hparser;
  std::vector<HarvestEntry*> entries;
  std::vector<std::string> tags;
  uint64_t num;
  gridfs gfs[1]; // the filesystem used to store images
  uint64_t slice_tweet_count;
  uint64_t slice_image_count;
  uint64_t slice_ends_on;
  uint64_t slice_duration;

};

class HarvesterThread : public roxlu::Runnable {
public:
  void setup();
  void run();

  bool getSliceCounts(uint64_t& numTweets, uint64_t& numImages);
  void resetSliceCounts();
  Mutex mutex;
  Harvester harvester;
  Thread thread;
};

#endif
