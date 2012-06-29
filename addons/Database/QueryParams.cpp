#include "QueryParams.h"

namespace roxlu {

QueryParams::QueryParams() {
}

QueryParams::~QueryParams() {
	vector<QueryParam*>::iterator it = params.begin();
	//printf("Number of query params: %zu\n", params.size());
	while(it != params.end()) {
		delete (*it);
		it = params.erase(it);
	}
}

QueryParams::QueryParams(const QueryParam& other) {
	*this = other;
}


QueryParams& QueryParams::operator=(const QueryParams& other) {
	//printf("Copying query params\n");
	if(&other == this) {
		return *this;
	}
	vector<QueryParam*>::const_iterator it = other.params.begin();
	while(it != other.params.end()) {
		params.push_back(new QueryParam(*it));
		++it;
	}
	return *this;
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

// used by insert query
// @todo we're calling setBindName here... maybe somewhere else (?)
// @todo probably better in the query class.
bool QueryParams::getValueList(string& result) {
	result.clear();
	vector<QueryParam*>::iterator it = params.begin();
	while(it != params.end()) {
		string bind_name = (*it)->getBindSQL();
		result.append(bind_name);
		++it;
		if(it != params.end()) {
			result.append(",");
		}
	}
	return result.length();
}

} // roxlu
