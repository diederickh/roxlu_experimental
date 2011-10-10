#include "Dictionary.h"
#include "DictionaryMap.h"
//#include "ofMain.h" // only for debugging

namespace roxlu {

//int Dictionary::num_maps = 0;


// Constructors
//------------------------------------------------------------------------------
Dictionary::Dictionary() {
	type = D_NULL;
	memset(&value, 0, sizeof(value));
}

Dictionary::~Dictionary() {
	reset();
}

Dictionary::Dictionary(const Dictionary& val) {
	copyFrom(val);
}

Dictionary::Dictionary(const bool& val) {
	type = D_BOOL;
	memset(&value, 0, sizeof(value));
	value.b = val;	
}

Dictionary::Dictionary(const int8_t &val) {
	type = D_INT8;
	memset(&value, 0, sizeof(value));
	value.i8 = val;
}

Dictionary::Dictionary(const int16_t &val) {
	type = D_INT16;
	memset(&value, 0, sizeof(value));
	value.i16 = val;
}

Dictionary::Dictionary(const int32_t &val) {
	type = D_INT32;
	memset(&value, 0, sizeof(value));
	value.i32 = val;
}

Dictionary::Dictionary(const int64_t &val) {
	type = D_INT64;
	memset(&value, 0, sizeof(value));
	value.i64 = val;
}

Dictionary::Dictionary(const uint8_t &val) {
	type = D_UINT8;
	memset(&value, 0, sizeof(value));
	value.ui8 = val;
}

Dictionary::Dictionary(const uint16_t &val) {
	type = D_UINT16;
	memset(&value, 0, sizeof(value));
	value.ui16 = val;
}

Dictionary::Dictionary(const uint32_t &val) {
	type = D_UINT32;
	memset(&value, 0, sizeof(value));
	value.ui32 = val;
}

Dictionary::Dictionary(const uint64_t &val) {
	type = D_UINT64;
	memset(&value, 0, sizeof(value));
	value.ui64 = val;
}

Dictionary::Dictionary(const double &val) {
	type = D_DOUBLE;
	memset(&value, 0, sizeof(value));
	value.d = val;
}

Dictionary::Dictionary(const string &val) {
	type = D_STRING;
	memset(&value, 0, sizeof(value));
	value.s = new string(val);
}

Dictionary::Dictionary(const char* val) {
	type = D_STRING;
	memset(&value, 0, sizeof(value));
	value.s = new string(val);
}

// Destructor
//------------------------------------------------------------------------------
void Dictionary::reset(bool isUndefined) {
	switch(type) {
		case D_BYTEARRAY:
		case D_STRING: {
			delete value.s;
			break;
		}
		case D_MAP: {
			delete value.m; 
			break;
		}
		case D_NULL: {
			break;
		}
		default: {
			//printf("@todo reset, check if all allocated types are supported!\n");
			break;
		}
	};
	if(isUndefined) {
		type = D_UNDEFINED;
	}
	else {
		type = D_NULL;
	}
		
	memset(&value, 0, sizeof(value));
}


// Operators
// -----------------------------------------------------------------------------
Dictionary& Dictionary::operator=(const Dictionary& other) {
	reset();
	copyFrom(other);
	return *this;
}

// we define this inlined function (so external usage will give unresolved)
void Dictionary::copyFrom(const Dictionary& other) {
	
	type = other.type;
	memset(&value, 0, sizeof(value));
	switch(other.type) {
		case D_BYTEARRAY:
		case D_STRING: {
			value.s = new string(*other.value.s);
			break;
		}
		case D_MAP: {	
			value.m = new DictionaryMap(*other.value.m);
			break;
		}
		default:{
			memcpy(&value, &other.value, sizeof(value));
			break;
		}
	};
}

	
Dictionary& Dictionary::operator=(const bool &val) {
	reset();
	type = D_BOOL;
	value.b = val;
	return *this;
}

Dictionary& Dictionary::operator=(const double &val) {
	reset();
	type = D_DOUBLE;
	value.d = val;
	return *this;
}

Dictionary& Dictionary::operator=(const string &val) {
	reset();
	type = D_STRING;
	value.s = new string(val);
	return *this;
}

Dictionary& Dictionary::operator=(const char* chars) {
	reset();
	type = D_STRING;
	value.s = new string(chars);
	return *this;
}


Dictionary& Dictionary::operator=(const int8_t &val) {
	reset();
	type = D_INT8;
	value.i8 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const int16_t &val) {
	reset();
	type = D_INT16;
	value.i16 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const int32_t &val) {
	reset();
	type = D_INT32;
	value.i32 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const int64_t &val) {
	reset();
	type = D_INT64;
	value.i64 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const uint8_t &val) {
	reset();
	type = D_UINT8;
	value.ui8 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const uint16_t &val) {
	reset();
	type = D_UINT16;
	value.ui16 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const uint32_t &val) {
	reset();
	type = D_UINT32;
	value.ui32 = val;
	return *this;
}

Dictionary& Dictionary::operator=(const uint64_t &val) {
	reset();
	type = D_UINT64;
	value.ui64 = val;
	return *this;
}

// @todo pass by ref?
bool Dictionary::operator==(Dictionary other) {
	return toString() == other.toString();
}

bool Dictionary::operator!=(Dictionary other) {
	return !operator==(other);
}

bool Dictionary::operator==(DictionaryType checkType) {
	if(checkType == D_NUMERIC) {
		return type == D_INT8 
				|| type == D_INT16
				|| type == D_INT32
				|| type == D_INT64
				|| type == D_UINT8
				|| type == D_UINT32
				|| type == D_UINT64
				|| type == D_DOUBLE;
				
	}	
	else  {
		return type == checkType;
	}
}

bool Dictionary::operator!=(DictionaryType checkType) {
	return !operator==(checkType);
}



Dictionary& Dictionary::operator[](const uint32_t& key) {
	stringstream ss;
	ss << VAR_INDEX_VALUE << key;
	return operator[](ss.str());
}

Dictionary& Dictionary::operator[](const string& key) {
	if(	(type != D_MAP) && (type != D_NULL) && (type != D_UNDEFINED)) {
		cout << "operator[]: Key index Applied to incorrect dictionary" << endl;
	}
	if(type == D_NULL || type == D_UNDEFINED) {
		type = D_MAP;
		//num_maps++; // tmp
		value.m = new DictionaryMap;
	}
	
	// add a new entry when it doesnt exist.
	if(!IN_MAP(value.m->children, key)) {
		value.m->children[key] = Dictionary();
	}
	
	// return reference to child.
	return value.m->children[key];
}

Dictionary& Dictionary::operator[](const char* key) {
	return operator[](string(key));
}

// type casting
//------------------------------------------------------------------------------
#define DICT_OPERATOR(ctype) \
	Dictionary::operator ctype() { \
		switch(type) { \
			case D_UNDEFINED: \
			case D_NULL: { \
				return 0; \
			} \
			case D_BOOL: { \
				return (ctype) value.b; \
			} \
			case D_INT8: { \
				return (ctype) value.i8; \
			} \
			case D_INT16: { \
				return (ctype) value.i16; \
			} \
			case D_INT32: { \
				return (ctype) value.i32; \
			} \
			case D_INT64: { \
				return (ctype) value.i64; \
			} \
			case D_UINT8: { \
				return (ctype) value.ui8; \
			} \
			case D_UINT16: { \
				return (ctype) value.ui16; \
			} \
			case D_UINT32: { \
				return (ctype) value.ui32; \
			} \
			case D_UINT64: { \
				return (ctype) value.ui64; \
			} \
			case D_DOUBLE: { \
				return (ctype) value.d; \
			} \
			case D_MAP: \
			case D_STRING: \
			default: { \
				printf("Dictionary cast failed.\n"); \
				return 0; \
			} \
		} \
	}  
			
//DICT_OPERATOR(bool);	
DICT_OPERATOR(double);
DICT_OPERATOR(int8_t);	
DICT_OPERATOR(int16_t);			
DICT_OPERATOR(int32_t);	
DICT_OPERATOR(int64_t);	
DICT_OPERATOR(uint8_t);	
DICT_OPERATOR(uint16_t);	
DICT_OPERATOR(uint32_t);	
DICT_OPERATOR(uint64_t);	

Dictionary::operator bool() {
	switch(type) {
		case D_NULL:
		case D_UNDEFINED: {
			return false;
		}
		
		case D_BOOL: {
			return value.b;
		}
		
		case D_INT8:
		case D_INT16:
		case D_INT32:
		case D_INT64:
		case D_UINT8:
		case D_UINT16:
		case D_UINT32:
		case D_UINT64:
		case D_DOUBLE: {
			bool result = false;
			result |= (value.i8 != 0);
			result |= (value.i16 != 0);
			result |= (value.i32 != 0);
			result |= (value.i64 != 0);
			result |= (value.ui8 != 0);
			result |= (value.ui16 != 0);
			result |= (value.ui32 != 0);
			result |= (value.ui64 != 0);
			result |= (value.d != 0); //!not in variant!
			return result;
		}
		
		case D_STRING:
		case D_MAP:
		default: {
			return false;
		}
	};
}

bool Dictionary::isNumeric() {
	switch(type) {
		case D_INT8:
		case D_INT16:
		case D_INT32:
		case D_INT64:
		case D_UINT8:
		case D_UINT16:
		case D_UINT32:
		case D_UINT64:
		case D_DOUBLE: {
			return true;
		}
		default: {
			return false;
		}
	}
	//return false;
}

bool Dictionary::isBoolean() {
	return type == D_BOOL;
}

Dictionary::operator string() {
	switch(type) {
		case D_BOOL: {
			return (value.m) ? "true" : "false";
		}
		
		case D_INT8: {
			stringstream ss;
			ss << value.i8;
			return ss.str();
		}
		
		case D_INT16: {
			stringstream ss;
			ss << value.i16;
			return ss.str();
		}
		
		case D_INT32: {
			stringstream ss;
			ss << value.i32;
			return ss.str();
		}
		
		case D_INT64: {
			stringstream ss;
			ss << value.i64;
			return ss.str();
		};
		
		case D_UINT8: {
			stringstream ss;
			ss << value.ui8;
			return ss.str();
		}
		
		case D_UINT16: {
			stringstream ss;
			ss << value.ui16;
			return ss.str();
		}
		
		case D_UINT32: {
			stringstream ss;
			ss << value.ui32;
			return ss.str();
		}
		
		case D_UINT64: {
			stringstream ss;
			ss << value.ui64;
			return ss.str();
		};
		
		case D_DOUBLE: {
			stringstream ss;
			ss << value.d;
			return ss.str();
		}
		
		case D_BYTEARRAY:
		case D_STRING: {
			return *value.s;
		}
		
		case D_UNDEFINED:
		case D_NULL:
		case D_MAP: 
		default: {
			//printf("Cannot cast to string, type: %d", type);
			return "";
		}
	};
	return "";
}


// Retrieve as forced type
//------------------------------------------------------------------------------
uint8_t Dictionary::getAsUInt8() {
	uint8_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

uint16_t Dictionary::getAsUInt16() {
	uint16_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

uint32_t Dictionary::getAsUInt32() {
	uint32_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

uint64_t Dictionary::getAsUint64() {
	uint64_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

int8_t Dictionary::getAsInt8() {
	int8_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

int16_t Dictionary::getAsInt16() {
	int16_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

int32_t  Dictionary::getAsInt32() {
	int32_t val;
	string as_str = (string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

int64_t  Dictionary::getAsInt64() {
	int64_t val;
	string as_str =(string)(*this);
	istringstream is(as_str);
	is >> val;
	return val;
}

// Serialization
//------------------------------------------------------------------------------
string Dictionary::toJSON() {
	string result = "";
	if(serializeToJSON(result)) {
		return result;
	}
	return "";
}

string Dictionary::toXML() {
	return toString();
}

bool Dictionary::serializeToJSON(string &result) {
	switch(type) {
		case D_NULL: {
			result += "null";
			break;
		}
		case D_BOOL: {
			result += ((bool)(*this)) ? "true" : "false";
			break;
		}
		case D_UINT8:
		case D_UINT16:
		case D_UINT32:
		case D_UINT64: 
		case D_INT8:
		case D_INT16:
		case D_INT32:
		case D_INT64: {
			int64_t val = (int64_t)(*this);
			stringstream ss;
			ss << val;
			result += ss.str();
			break;
		}

		case D_DOUBLE: {
			stringstream ss;
			ss << setprecision(4) << value.d;
			result += ss.str();
			break;
		}
		
		case D_STRING: {
			string str = (string)(*this);
			escapeJSON(str);
			result += str;
			break;
		}
		case D_BYTEARRAY: {
			result += "\"bytearray not supported\"";
			break;
		}
		case D_MAP: {
			result += (isArray()) ? "[" : "{";

			// serialze all children.
			map<string, Dictionary>::iterator it = value.m->children.begin();
			while(it != value.m->children.end()) {
				// get key.
				string map_key = it->first;
				if(!isArray()) {
					escapeJSON(map_key);
					result += map_key +":";
				}	
				// get values.
				Dictionary map_val = it->second;
				if(!map_val.serializeToJSON(result)) {
					printf("Error while converting to json for key: %s\n", map_key.c_str());
					return false;
				}
				result += ",";
				++it;
			}	
			
			// close
			if(value.m->children.size() > 0) {
				result[result.size()-1] = (isArray()) ? ']' :'}';
			}
			else {
				result += (isArray()) ? "]" : "}";
			}
			break;
		}
		default: {
			printf("Unhandled type which we cannot serialize");
			break;
		};
	};
	return true;
}

void Dictionary::escapeJSON(string& v) {
	replaceString(v, 	"\\",	"\\\\");
	replaceString(v, 	"/", 	"\\/"); 
	replaceString(v, 	"\"", 	"\\\""); 
	replaceString(v, 	"\b", 	"\\b"); 
	replaceString(v, 	"\f", 	"\\f"); 
	replaceString(v, 	"\n", 	"\\n"); 
	replaceString(v, 	"\r", 	"\\r"); 
	replaceString(v, 	"\t", 	"\\t"); 
	v = "\"" +v +"\"";
}

bool Dictionary::toBinary(IOBuffer& buffer) {
	buffer.storeByte(type);
	switch(type) {
		case D_NULL:
		case D_UNDEFINED: {
			return true;
			break;
		}
		case D_BOOL: {
			buffer.storeByte((uint8_t)value.b);
			return true;
			break;
		}
		case D_INT8: {
			buffer.storeByte(value.i8);
			return true;
			break;
		}
		case D_INT16: {
			buffer.storeBigEndianUI16(value.i16);
			return true;
			break;
		}
		case D_INT32: {
			buffer.storeBigEndianUI32(value.i32);
			return true;
			break;
		}
		case D_INT64: {
			buffer.storeBigEndianUI64(value.i64);
			return true;
			break;
		}
		case D_UINT8: {
			buffer.storeByte(value.ui8);
			return true;
			break;
		}
		case D_UINT16: {
			buffer.storeBigEndianUI16(value.ui16);
			return true;
			break;
		}
		case D_UINT32: {
			buffer.storeBigEndianUI32(value.ui32);
			return true;
			break;
		}
		case D_UINT64: {
			buffer.storeBigEndianUI64(value.ui64);
			return true;
			break;
		}
		case D_DOUBLE: {
			buffer.storeBigEndianDouble(value.d);
			return true;
			break;
		}
		case D_STRING: {
			buffer.storeStringWithSize(*value.s);
			return true; 
			break;
		}
		case D_MAP: {
			bool is_array = isArray();
			buffer.storeByte(is_array);
			
			uint32_t length = getMapSize();
			buffer.storeBigEndianUI32(length);
			
			Dictionary::iterator it = begin();
			while(it != end()) {
				buffer.storeStringWithSize(it->first);
				IOBuffer recursed_buffer;
				if(!it->second.toBinary(recursed_buffer)) {
					printf("Dictionary.toBinary(): unable to serialize dictionary\n");
					return false;
				}
				else {
					buffer.storeBuffer(recursed_buffer);
				}
				++it;
			}
			return true;
			break;
		}
		
		default: {
			printf("Dictionary.toBinary(): type not handled: %u\n", type);
			break;
		}
	};
	return true;
}

bool Dictionary::fromBinary(IOBuffer& buffer) {
	return fromBinaryInternal(buffer, *this);
}
bool Dictionary::fromBinaryInternal(IOBuffer& buffer, Dictionary& result) {
	uint8_t stored_type = buffer.consumeByte();
	switch(stored_type) {
		case D_NULL: {
			result.reset(false);
			return true;
		}
		case D_UNDEFINED: {
			result.reset(true);
			return true;
		}
		case D_BOOL: {
			result = (bool)buffer.consumeByte();
			return true;
		}
		case D_INT8: {
			result = (int8_t)buffer.consumeByte();
			return true;
		}
		case D_INT16: {
			result = (int16_t)buffer.consumeBigEndianI16();
			return true;
		}
		case D_INT32: {
			result = (int32_t)buffer.consumeBigEndianI32();
			return true;
		}
		case D_UINT8: {
			result = (uint8_t)buffer.consumeByte();
			return true;
		}
		case D_UINT16: {
			result = (uint16_t)buffer.consumeBigEndianUI16();
			return true;
		}
		case D_UINT32: {
			result = (uint32_t)buffer.consumeBigEndianUI32();
			return true;
		}
		case D_UINT64: {
			result = (uint64_t)buffer.consumeBigEndianUI64();
			return true;
		}
		case D_DOUBLE: {
			result = (double)buffer.consumeBigEndianDouble();
			return true;
		}
		case D_STRING: {
			result = (string)buffer.consumeStringWithSize();
			return true;
		}
		case D_MAP: {
			bool is_array = (bool)buffer.consumeByte();
			result.isArray(is_array);
			uint32_t length = buffer.consumeBigEndianUI32();
			for(uint32_t i = 0; i < length; i++) {
				string key = buffer.consumeStringWithSize();
				if(!Dictionary::fromBinaryInternal(buffer, result[key])) {
					printf("Dictionary.fromBinary: cannot deserialize map for key: %s\n", key.c_str());
					return false;
				}
			}
			return true;
		}
		default: {
			printf("Dictionary.fromBinary, unhandled type: %02X\n", stored_type);
			return false;
		}
		
	};
	return true;
}

// String functions.
//------------------------------------------------------------------------------
void Dictionary::replaceString(string& target, string search, string replacement) {
	if (search == replacement) {
		return;
	}
	if (search == "") {
		return;
	}
	string::size_type i = string::npos;
	string::size_type last_pos = 0;
	while ((i = target.find(search, last_pos)) != string::npos) {
		target.replace(i, search.length(), replacement);
		last_pos = i + replacement.length();
	}
}

string Dictionary::toString(string name, uint32_t indent) {
	string result = "";
	string str_indent = string(indent*4, ' ');
	switch(type) {
		case D_NULL: {
			result += str_indent +"<NULL name=\"" +name +"\"></NULL>";
			break;
		}
		case D_UNDEFINED: {
			result += str_indent +"<UNDEFINED name=\"" +name +"\"></UNDEFINED>";
			break;
		}
		case D_BOOL: {
			result += str_indent +"<BOOL name=\"" +name +"\">" 
										+((value.b) ? "true":"false") 
								+"</BOOL>";
			break;
		}
		case D_INT8: {
			stringstream ss;
			ss << value.i8;
			result += str_indent +"<INT8 name=\"" +name +"\">" +ss.str() +"</INT8>";
			break;
		}
		case D_INT16: {
			stringstream ss;
			ss << value.i16;
			result += str_indent +"<INT16 name=\"" +name +"\">" +ss.str() +"</INT16>";
			break;
		}
		case D_INT32: {
			stringstream ss;
			ss << value.i32;
			result += str_indent +"<INT32 name=\"" +name +"\">" +ss.str() +"</INT32>";
			break;
		}
		case D_INT64: {
			stringstream ss;
			ss << value.i64;
			result += str_indent +"<INT64 name=\"" +name +"\">" +ss.str() +"</INT64>";
			break;
		}
		case D_UINT8: {
			stringstream ss;
			ss << value.ui8;
			result += str_indent +"<UINT8 name=\"" +name +"\">" +ss.str() +"</UINT8>";
			break;
		}
		case D_UINT16: {
			stringstream ss;
			ss << value.ui16;
			result += str_indent +"<UINT16 name=\"" +name +"\">" +ss.str() +"</UINT16>";
			break;
		}
		case D_UINT32: {
			stringstream ss;
			ss << value.ui32;
			result += str_indent +"<UINT32 name=\"" +name +"\">" +ss.str() +"</UINT32>";
			break;
		}
		case D_UINT64: {
			stringstream ss;
			ss << value.ui64;
			result += str_indent +"<UINT64 name=\"" +name +"\">" +ss.str() +"</UINT64>";
			break;
		}
		case D_DOUBLE: {
			stringstream ss;
			ss << value.d;
			result += str_indent +"<DOUBLE name=\"" +name +"\">" +ss.str() +"</DOUBLE>";
			break;
		}
		case D_STRING: {
			result += str_indent +"<STR name=\"" +name +"\">" +(*value.s) +"</STR>";
			break;
		}
		case D_BYTEARRAY: {
			stringstream ss;
			ss << (*value.s).size();
			result += str_indent +"<BYTEARRAY name=\"" +name +"\">" +ss.str() +" bytes</BYTEARRAY>";
			break;
		}
		case D_MAP: {
			map<string, Dictionary>::iterator it = value.m->children.begin();
			result += str_indent +"<MAP name=\"" +name +"\" is_array=\"" +((value.m->is_array) ? "true":"false") +"\">\n";
			while(it != value.m->children.end()) {
				result += it->second.toString((string)it->first, indent+1) +"\n";
				++it;
			}
			result += str_indent +"</MAP>";
			break;
		}
		default: {
			printf("dictionary: cannot convert unhandled type toString()\n");
			break;
		}
		
	}
	return result;
}

// Array related 
// -----------------------------------------------------------------------------
bool Dictionary::isArray() {
	if(type == D_MAP) {
		return value.m->is_array;
	}
	return false;
}

void Dictionary::isArray(bool makeArray) {
	if(type == D_NULL) {
		
		type = D_MAP;
		value.m = new DictionaryMap;
		//num_maps++;
		//cout << "Map created (2):" << num_maps << " we are:" << this<< endl;
	}
	if(type == D_MAP) {
		value.m->is_array = makeArray;
	}
}

void Dictionary::isByteArray(bool makeByteArray) {	
	if(makeByteArray) {
		if(type == D_STRING) {
			type = D_BYTEARRAY;
		}
	}
	else {
		if(type == D_BYTEARRAY) {
			type = D_STRING;
		}
	}
}

bool Dictionary::isByteArray() {
	return type == D_BYTEARRAY;
}

uint32_t Dictionary::getMapSize() {
	if(type == D_NULL || type == D_UNDEFINED) {
		return 0;
	}
	if(type != D_MAP) {
		printf("cannot get map size, we are not a map\n");
		return 0;
	}
	return (uint32_t) value.m->children.size();

}

// size of 'none' name=value pairs (so total number of alements which 
// are indexed by a uint32_t. Internally these are stored using a 
// special marker key. See the  "VAR_INDEX_VALUE" define. 
uint32_t Dictionary::getMapDenseSize() {
	if(type == D_NULL || type == D_UNDEFINED) {
		return 0;
	}
	if(type != D_MAP) {
		printf("cannot get map dense size, we are not a map\n");
		return 0;
	}
	uint32_t count = 0;
	uint32_t s = getMapSize();
	for(count = 0; count < s; ++count) {
		stringstream ss;
		ss << VAR_INDEX_VALUE << count;
		string dx_str = ss.str(); 
		if(!IN_MAP(value.m->children, dx_str)) {
			break;
		}
	}
	return count;
}

void Dictionary::pushToArray(Dictionary dict) {
	if(type != D_NULL && type != D_MAP) {
		printf("cannot push to array we are not a map\n");
		return;
	}
	isArray(true);
	(*this)[(uint32_t)this->getMapDenseSize()] = dict;
}

void Dictionary::removeKey(const string& key) {
	if(type != D_MAP) {
		printf("cannot removeKey(), we are not a map.\n");
		return;
	}
	value.m->children.erase(key);
}

void Dictionary::removeAt(const uint32_t index) {
	if(type != D_MAP) {
		printf("cannot removeAt(), we are not a map.\n");
		return;
	}
	stringstream ss;
	ss << VAR_INDEX_VALUE << index;
	value.m->children.erase(ss.str());
}

bool Dictionary::hasKey(const string key) {
	if(type != D_MAP) {
		return false;
	}
	return IN_MAP(value.m->children, key);
}

// Iterate over values.
//------------------------------------------------------------------------------
map<string, Dictionary>::iterator Dictionary::begin() {
	if(type != D_MAP) {
		map<string,Dictionary> tmp;
		return tmp.begin();
	}
	return value.m->children.begin();
}

map<string, Dictionary>::iterator Dictionary::end() {
	if(type != D_MAP) {
		map<string,Dictionary> tmp;
		return tmp.end();
	}
	return value.m->children.end();
}

	
} // namespace roxlu