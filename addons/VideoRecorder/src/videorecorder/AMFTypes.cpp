#include <videorecorder/AMFTypes.h>


// A M F  C O N T A I N E R
// -------------------------------------------------------
AMFContainer::AMFContainer()
	:AMFType(AMF_TYPE_NOT_SET)
{
}

AMFContainer::AMFContainer(int type)
	:AMFType(type)
{
}

AMFContainer::~AMFContainer() {
	/* 2012.10.11, we're not responsible for the memory
	for(int i = 0; i < elements.size(); ++i) {
		delete elements[i];
	}
	*/
}

void AMFContainer::deleteElements() {
	for(int i = 0; i < elements.size(); ++i) {
		delete elements[i];
	}
}

void AMFContainer::push_back(AMFType* type) {
	elements.push_back(type);
}

std::vector<AMFType*>::iterator AMFContainer::find(const std::string& name) {
	for(std::vector<AMFType*>::iterator it = elements.begin(); it != elements.end(); ++it) {
		AMFType& t = **it;
		if(t.type == AMF0_TYPE_STRING) {
			AMFString* st = static_cast<AMFString*>(&t);
			if(st->getValue() == name) {
				return it;
			}
		}
	}
	return elements.end();
}

void AMFContainer::print() {
	for(std::vector<AMFType*>::iterator it = elements.begin(); it != elements.end(); ++it) {
		(*it)->print();
	}
}


// G E N E R I C   A M F  T Y P E S
// -------------------------------------------------------
AMFType::AMFType(int type)
       :type(type)
{
}

AMFType::~AMFType() {
	//	printf("~AMFType()\n");
}

void AMFType::print() {
}

int AMFType::getType() {
	return type;
}

// Number
// -----------------------------------------
AMFNumber::AMFNumber(double value)
	:AMFType(AMF0_TYPE_NUMBER)
	,value(value)
{
}

void AMFNumber::print() {
	printf("AMFNumber.value = %f\n", value);
}

// String
// -----------------------------------------
AMFString::AMFString(const std::string& str, bool writeTag)
	:AMFType(AMF0_TYPE_STRING)
	,value(str)
	,write_tag(writeTag)
{
}
AMFString::~AMFString() {
	//printf("~AMFString()\n");
}

void AMFString::print() {
	printf("AMFString.value = '%s'\n", value.c_str());
}

std::string AMFString::getValue() {
	return value;
}

// Boolean
// -----------------------------------------
AMFBoolean::AMFBoolean(bool value)
	:AMFType(AMF0_TYPE_BOOLEAN)
	,value(value)
{
}

void AMFBoolean::print() {
	printf("AMFBoolean.value = %c\n", (value) ? 'y' : 'n');
}

bool AMFBoolean::getValue() {
	return value;
}

// Object
// -----------------------------------------
AMFObject::AMFObject()
	:AMFType(AMF0_TYPE_OBJECT)
{
}

// MovieClip
// -----------------------------------------
AMFMovieClip::AMFMovieClip()
	:AMFType(AMF0_TYPE_MOVIECLIP)
{
}

// Null
// -----------------------------------------
AMFNull::AMFNull()
	:AMFType(AMF0_TYPE_NULL)
{
}

// Undefined
// -----------------------------------------
AMFUndefined::AMFUndefined()
	:AMFType(AMF0_TYPE_UNDEFINED)
{
}

// Reference
// -----------------------------------------
AMFReference::AMFReference()
	:AMFType(AMF0_TYPE_REFERENCE)
{
}

// ECMA Array
// -----------------------------------------
AMFEcmaArray::AMFEcmaArray() 
	:AMFContainer(AMF0_TYPE_ECMA_ARRAY)
{	
}

void AMFEcmaArray::operator()(const std::string& name, AMFType* el) {
	// find the element with this name
	std::vector<AMFType*>::iterator it =  this->find(name);
	if(it == elements.end()) {
		AMFString* name_el = new AMFString(name);
		push_back(name_el); // name type
		push_back(el);    // value type
	}
	else {
		++it; // get value el.
		//delete *it; // 2012.10.11, we are not responsible for handling memory
		int dx = it - elements.begin();
		elements[dx] = el;
	}
}


// Object end
// -----------------------------------------
AMFObjectEnd::AMFObjectEnd()
	:AMFType(AMF0_TYPE_OBJECT_END)
{
}

void AMFObjectEnd::print() {
	printf("AMFObjectEnd.value <>\n");
}

// Strict array
// -----------------------------------------
AMFStrictArray::AMFStrictArray()
	:AMFType(AMF0_TYPE_STRICT_ARRAY)
{
}

// Date
// -----------------------------------------
AMFDate::AMFDate()
	:AMFType(AMF0_TYPE_DATE)
{
}

// Long string
// -----------------------------------------
AMFLongString::AMFLongString()
	:AMFType(AMF0_TYPE_LONG_STRING)
{
}

// XML
// -----------------------------------------
AMFXML::AMFXML()
	:AMFType(AMF0_TYPE_XML)
{
}

// Typed object
// -----------------------------------------
AMFTypedObject::AMFTypedObject()
	:AMFType(AMF0_TYPE_TYPED_OBJECT)
{

}

// -----------------------------------------
// -----------------------------------------
// -----------------------------------------
// -----------------------------------------

