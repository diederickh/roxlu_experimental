#include "Collection.h"

namespace roxlu {
namespace curl {
namespace parameter {

namespace rcp = roxlu::curl::parameter;

Collection::Collection() {
}

Collection::~Collection() {
	clear();	
}


Collection::Collection(const Collection& other) {
	*this = other;
}

Collection& Collection::operator=(const Collection& other) {
	clear();
	*this += other;
	return *this;
}

void Collection::clear() {
	list<Parameter*>::iterator it = params.begin();
	while(it != params.end()) {
		delete *it;
		it = params.erase(it);
	}
}

bool Collection::removeParameter(const string& name) {
	list<Parameter*>::iterator it = params.begin();
	while(it != params.end()) {
		if((*it)->getName() == name) {
			delete (*it);
			params.erase(it);
			return true;
		}
		++it;
	}
	return false;
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
	rcp::File* f = new File(name, file);
	params.push_back(f);
	return f;
}

string Collection::getQueryString() {
	string ps;
	if(params.size() > 0) {
		ps.append("?");
	}
	
	list<rcp::Parameter*>::const_iterator it = params.begin();
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
list<rcp::Parameter*> Collection::getParameters(bool forSignature) const{
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