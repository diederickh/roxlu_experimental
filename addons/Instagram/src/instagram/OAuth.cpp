#include <instagram/OAuth.h>
namespace roxlu {

OAuth::OAuth() {
}

OAuth::~OAuth() {
}

void OAuth::setup(const std::string consumerKey, std::string consumerSecret) {
  consumer_key = consumerKey;
  consumer_secret = consumerSecret;
}

} // roxlu
