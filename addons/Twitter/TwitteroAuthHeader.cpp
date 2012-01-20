#include "TwitteroAuthHeader.h"
//#include "TwitteroAuthSignature.h"

namespace roxlu {

//TwitteroAuthHeader::TwitteroAuthHeader(TwitteroAuthSignature& s)
//	:sig(s)
TwitteroAuthHeader::TwitteroAuthHeader()

{
}


// Create the header string.
// https://dev.twitter.com/docs/auth/authorizing-request
string TwitteroAuthHeader::getHeader(const map<string, string>& varvals) {
	
	list<string> keylist;
	string result;
	result.assign("Authorization: OAuth ");
	
	// create key-value pairs, with quotes.
	// -------------------------------------
	{
		map<string, string>::const_iterator it = varvals.begin();
		string varval;
		while(it != varvals.end()) {
			varval.assign(it->first);
			varval.append("=");
			varval.append("\"");
			varval.append(it->second);
			varval.append("\"");
			keylist.push_back(varval);
			++it;
		}
	}
	
	
	// After sorting append var=value strings
	// ---------------------------------------
	keylist.sort(); // IMPORTANT FOR OAUTH
	{
		list<string>::const_iterator it = keylist.begin();
		while(it != keylist.end()) {
			result.append(*it);
			++it;
			if(it != keylist.end()) {
				result.append(", ");
			}
		}
	}
	return result;
}

}; // roxlu