#include "QueryParam.h"
#include "Database.h"

namespace roxlu {

QueryParam::QueryParam() 
	:field("")
	,value("")
	,bind_name("")
	,type(SQL_PARAM_TEXT)
{
}

string QueryParam::getField() {
	return field;
}

int QueryParam::getType() {
	return type;
}

string QueryParam::getBindName() {
	return bind_name;
}

void QueryParam::setBindName(const string& name) {
	printf("> %s\n", name.c_str());
	bind_name = name;
}

string QueryParam::getValue() {
	return value;
}

} // roxlu
