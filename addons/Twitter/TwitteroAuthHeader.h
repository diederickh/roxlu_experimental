#ifndef ROXLU_TWITTEROAUTHHEADERH
#define ROXLU_TWITTEROAUTHHEADERH

#include <string>
#include <map>
#include <list>

using std::string;
using std::map;
using std::list;

namespace roxlu {

//class TwitteroAuthSignature;

class TwitteroAuthHeader {
public:
	TwitteroAuthHeader();
	string getHeader(const map<string, string>& varvals);
private:
//	TwitteroAuthSignature& sig;
};


}; // roxlu

#endif
