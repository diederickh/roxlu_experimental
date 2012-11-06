#ifndef ROXLU_HARVESTERH
#define ROXLU_HARVESTERH

#include <vector>
#include <algorithm>
#include <roxlu/Roxlu.h>
#include <twitter/Twitter.h>
#include <Kurl/Kurl.h>
#include <Types.h>
#include <jansson.h>
#include <http_parser/http_parser.h>

extern "C" {
#include <mongo.h>
#include <bson.h>
}

// Harvesting real time data from
// - twitter
enum HarvestTypes {
  HT_TWEET
};



class Harvester;
struct HarvestEntry {
  std::string id_str;
  std::string media_id_str;
  int type;
  Harvester* h;
};

class Harvester {
public:
  Harvester();
  ~Harvester();
  void setup();
  void update();
private:
  static void onTweet(const char* data, size_t len, void* harv);
  static void onTweetImageDownloadComplete(KurlConnection* c, void* harv);
public:
  roxlu::twitter::Twitter tw;
  mongo mcon;
  Kurl kurl;
  http_parser hparser;
  std::vector<HarvestEntry*> entries;
  std::vector<std::string> tags;
  uint64_t num;
};
#endif
