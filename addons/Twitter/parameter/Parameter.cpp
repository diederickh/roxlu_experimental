#include "Parameter.h"

namespace roxlu {
namespace twitter {
namespace parameter {

Parameter::Parameter() 
	:type(PARAM_STRING)
{
}

Parameter::Parameter(Parameter* other) 
	:type(other->type)
	,name(other->name)
	,value(other->value)
{
}

Parameter::Parameter(const std::string& n, const std::string& v) 
	:name(n)
	,value(v)
	,type(PARAM_STRING)
{
}

Parameter::~Parameter() {
	//printf("~ Parameter()\n");
}

void Parameter::print() {
	printf("%s = '%s'", getName().c_str(), getStringValue().c_str());
}

bool Parameter::mustUseInSignature() {
	return true;
}



}}} // roxlu::twitter::parameter