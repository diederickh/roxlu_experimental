#ifndef ROXLU_TWITTER_PARAMETER_COLLECTIONH
#define ROXLU_TWITTER_PARAMETER_COLLECTIONH

#include <string>
#include <vector>
#include <list>
#include <map>

#include "Parameter.h"
#include "File.h"
//#include "String.h"

using std::list;
using std::vector;
using std::string;
using std::map;

namespace roxlu {
namespace curl {
namespace parameter {

namespace rcp = roxlu::curl::parameter;

/**
 * A collection of Parameter* values. This can be used to i.e. create a 
 * Request for twitter. A collection is shared with i.e. the Signature and
 * Header classes of oAuth.
 *
 *
 */
class Collection {
public:
	Collection();
	~Collection();
	Collection(const Collection& other);
	Collection& operator=(const Collection& other);
	
	// Add a string var-value
	template<typename T>
	Parameter* addString(const string& name, const T& value) {
		Parameter* v = new Parameter(name, "");
		v->setStringValue(value);
		params.push_back(v);
		return v;
	}
	
	File* addFile(const string& name, const string& file);
	
	bool removeParameter(const string& name);
	Parameter* findByName(const string& name);
	Parameter& operator[](const string& name);
	Collection& operator+=(const Collection& other);
	
	string getQueryString();
	
	void print() const;
	const list<Parameter*>& getParameters() const;
	list<rcp::Parameter*> getParameters(bool forSignature) const;
	void clear(); // remove all params 	
private:
	list<Parameter*> params;
};

inline Parameter* Collection::findByName(const string& name) {
	list<Parameter*>::iterator it = params.begin();
	while(it != params.end()) {
		if((*it)->getName() == name) {
			return *it;
		}
		++it;
	}
	return NULL;
	
}

inline Parameter& Collection::operator[](const string& name) {
	Parameter* param = findByName(name);
	if(param != NULL) {
		return *param;
	}
	else {
		Parameter* p = new Parameter();
		p->setName(name);
		params.push_back(p);
		return *p;
	}
}


inline Collection& Collection::operator+=(const Collection& other) {
	list<Parameter*>::const_iterator it = other.params.begin();
	
	while(it != other.params.end()) {
		switch((*it)->type) {
			case Parameter::PARAM_STRING: {
				rcp::Parameter* p = new rcp::Parameter((*it));
				params.push_back(p);
				break;
			};
			case Parameter::PARAM_FILE: {
				rcp::File* f = new rcp::File((rcp::File*)(*it));
				params.push_back(f);
				break;
			}
			default: {
				printf("!!! Unhandled type in Collection += operator! => %d\n !!!", (*it)->type);
				break;
			};
		}
		++it;
	}
	
	return *this;
}

inline const list<Parameter*>& Collection::getParameters() const {
	return params;
}


}}} // roxlu::curl::parameter

#endif