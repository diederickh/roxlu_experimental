#include "QueryParams.h"

namespace roxlu {

QueryParams::QueryParams() {
}

QueryParams::~QueryParams() {
}

bool QueryParams::getFieldList(string& result) {
	result.clear();
	vector<QueryParam*>::iterator it = params.begin();
	while(it != params.end()) {
		result.append((*it)->getField());
		++it;
		if(it != params.end()) {
			result.append(",");
		}
	}
	return result.length();
}

// @todo we're calling setBindName here... maybe somewhere else (?)
// @todo probably better in the query class.
bool QueryParams::getValueList(string& result) {
	result.clear();
	vector<QueryParam*>::iterator it = params.begin();
	while(it != params.end()) {
		string bind_name = ":" +(*it)->getField();
		result.append(bind_name);
		(*it)->setBindName(bind_name);	
		++it;
		if(it != params.end()) {
			result.append(",");
		}
	}
	return result.length();
}

} // roxlu
