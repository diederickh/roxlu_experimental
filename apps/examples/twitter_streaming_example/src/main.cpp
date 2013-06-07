#include <iostream>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <twitter/Twitter.h>

// gets called when we receive new data
void twitter_filter_cb(HTTPConnection* c, HTTPConnectionEvent event, 
                               const char* data, size_t len, void* user) 
{
  if(event == HTTP_ON_STATUS) {
    RX_VERBOSE("HTTP status: %d", c->parser.status_code);
  }
  else if(event == HTTP_ON_BODY) {
    // parse the tweet.
    std::string str(data, data+len);
    Tweet tweet;
    tweet.parseJSON(str);
    tweet.print();
  }
}

int main() {

  Twitter tw;
  bool r =  tw.setup("466622389-...",  /* the token */
                     "eH25IAxRIB...",  /* the token secret */
                     "e0vURm6xhS...",  /* the consumer key */
                     "R7HfL0vgy...."); /* the consumer secret */
  if(!r) {
    RX_ERROR("Cannot setup the twitter obj");
    ::exit(EXIT_FAILURE);
  }

  TwitterStatusesFilter tw_filter;
  tw_filter.track("love");
  tw.apiStatusesFilter(tw_filter, twitter_filter_cb, NULL);
  
  while(true) {
    tw.update();
  }

  return 0;
};

