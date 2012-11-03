#ifndef ROXLU_HARVESTERH
#define ROXLU_HARVESTERH

#include <roxlu/Roxlu.h>
#include <twitter/Twitter.h>
#include <formats/json.h>
#include <Types.h>

extern "C" {
#include <mongo.h>
#include <bson.h>
}

// Harvesting real time data from
// - twitter

class Harvester {
public:
  Harvester();
  ~Harvester();
  void setup();
  void update();
private:
  static void onTweet(const char* data, size_t len, void* harv);
private:
  roxlu::twitter::Twitter tw;
  mongo mcon;
};
#endif
