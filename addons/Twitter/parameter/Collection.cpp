#include "Collection.h"

namespace roxlu {
namespace twitter {
namespace parameter {

namespace rtp = roxlu::twitter::parameter;

Collection::Collection() {
}

Collection::~Collection() {
	list<Parameter*>::iterator it = params.begin();
	while(it != params.end()) {
		delete *it;
		it = params.erase(it);
	}	
}

void Collection::print() const {
	list<Parameter*>::const_iterator it = params.begin();
	while(it != params.end()) {
		(*it)->print();
		printf("\n");
		++it;
	}	
}

File* Collection::addFile(const string& name, const string& file) {
	rtp::File* f = new File(name, file);
	params.push_back(f);
	return f;
}

string Collection::getQueryString() {
	string ps;
	if(params.size() > 0) {
		ps.append("?");
	}
	
	list<rtp::Parameter*>::const_iterator it = params.begin();
	while(it != params.end()) {
		ps.append((*it)->getName());
		ps.append("=");
		ps.append((*it)->getStringValue());
		++it;
		if(it != params.end()) {
			ps.append("&");
		}	
	}
	return ps;
}


/** 
 * Get only parameters that are either used or not-used to create the 
 * signature for oauth. In short multi-part form data is not included
 * in the signature base.  
 *
 * @param	bool			Pass true when you want only parameters 
 *							that are used in the signature.
 */
list<rtp::Parameter*> Collection::getParameters(bool forSignature) const{
	list<Parameter*> result;
	list<Parameter*>::const_iterator it = params.begin();
	while(it != params.end()) {
		if((*it)->mustUseInSignature() == forSignature) {
			result.push_back(*it);
		}
		++it;
	}
	return result;
}


}}} // roxlu::twitter::parameter