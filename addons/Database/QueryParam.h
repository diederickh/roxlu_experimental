#ifndef ROXLU_DATABASE_QUERY_PARAMH
#define ROXLU_DATABASE_QUERY_PARAMH

#include "sqlite3.h"
#include <string>
#include <sstream>

using std::string;
using std::stringstream;

namespace roxlu {

// default query param for string values.
class QueryParam {
public:
	enum QueryParamTypes {
		SQL_PARAM_TEXT
	};
	
	QueryParam();
	~QueryParam();
	
	template<typename T>
	QueryParam& use(const string& fieldName, const T& fieldValue) {
		stringstream ss;
		ss << fieldValue;
		value = ss.str();
		field = fieldName;
		return *this;
	}
	
	virtual int getType();
	
	string getField();
	string getValue();
	string getBindName();
	void setBindName(const string& name);

protected:
	int type;
	string bind_name;
	string field;
	string value;
	
};



}

#endif
