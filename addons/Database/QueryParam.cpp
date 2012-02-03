#include "QueryParam.h"
#include "Database.h"

namespace roxlu {

QueryParam::QueryParam() 
	:field("")
	,value("")
	,type(SQL_PARAM_TEXT)														
{
	//printf("Created QueryParam %p\n", this);
}

QueryParam::~QueryParam() {
	//printf("removing: %s\n", field.c_str());
	//printf("~ QueryParam\n");
}

QueryParam::QueryParam(const QueryParam* other) {
	*this = *other;
}

QueryParam& QueryParam::operator=(const QueryParam& other) {
	if(this == &other) {
		return *this;
	}
	
	type = other.type;
	field = other.field;
	value = other.value;
	return *this;
}

string QueryParam::getField() {
	return field;
}

int QueryParam::getType() {
	return type;
}

void QueryParam::setType(QueryParamType qpType) {
	type = qpType;
}

string QueryParam::getBindName() {
	string result = "";
	result.append(":");
	result.append(field);
	return result;
}

string QueryParam::getBindSQL() {
	string result;
		switch(type) {
		case SQL_PARAM_TEXT: {
			result.append(":");
			result.append(field);
			break;
		}
		
		case SQL_PARAM_TIMESTAMP: {
			result.append("strftime('%s','now')");
			break;
		}
		
		case SQL_PARAM_DATETIME: {
			result.append("datetime('now')");
			break;

		}
		
		default: {
			break;
		}
	}
	return result;
}

string QueryParam::getValue() {
	return value;
}

} // roxlu
