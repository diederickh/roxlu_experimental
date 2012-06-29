#include "Header.h"

namespace roxlu {
namespace curl {
namespace oauth {

/**
 * Creates the oAuth header for authorized request to twitter
 *
 * 
 * @see https://dev.twitter.com/docs/auth/creating-signature 
 */
string Header::getHeader(const rcp::Collection& col) {
	string result;
	//result.assign("Authorization: OAuth ");
	result.assign("OAuth ");
	list<string> keylist;
	const list<rcp::Parameter*>& pars = col.getParameters();
	
	// create key-value pairs, with quotes.
	// -------------------------------------
	{
		list<rcp::Parameter*>::const_iterator it = pars.begin();
		string varval;
		while(it != pars.end()) {
			varval.assign((*it)->getName());
			varval.append("=");
			varval.append("\"");
			varval.append((*it)->getStringValue());
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
				result.append(", "); // comma space!
			}
		}
	}
	return result;
}	

}}};