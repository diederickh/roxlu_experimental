#ifndef ROXLU_DATABASE_QUERY_PARAMSH
#define ROXLU_DATABASE_QUERY_PARAMSH

#include "QueryParam.h"
#include <vector>
#include "sqlite3.h"

using std::vector;
using std::string;

// collection of query parameters
namespace roxlu {

class QueryParams {
public:
	QueryParams();
	~QueryParams();
	
	template<typename T>
	QueryParams& use(const string& fieldName, const T& fieldValue) {
		printf("set field: %s\n", fieldName.c_str());
		QueryParam* qp = new QueryParam();
		qp->use(fieldName, fieldValue);
		params.push_back(qp);
		return *this;
	}
	
	const vector<QueryParam*>& getParams();
	
	bool getFieldList(string& result);
	bool getValueList(string& result);

	
private:
	vector<QueryParam*> params;
};

inline const vector<QueryParam*>& QueryParams::getParams() {
	return params;
}


} // roxlu

#endif
