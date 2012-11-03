#ifndef ROXLU_TWITTER_PERCENT_ENCODEH
#define ROXLU_TWITTER_PERCENT_ENCODEH

#include <iostream>
#include <string>
#include <ctype.h>
namespace roxlu {
  namespace twitter {
class PercentEncode {
public:
  PercentEncode();
  std::string encode(std::string input);
private:
  char rfc3986[256];
};
  } // roxlu
} // twitter
#endif
