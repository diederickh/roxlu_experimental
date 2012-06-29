#ifndef OSCARGH
#define OSCARGH

#include <string>
using std::string;


// Type identifiers
typedef enum __OSCArgType {
	 OSCARG_TYPE_NONE
	,OSCARG_TYPE_INT32
	,OSCARG_TYPE_FLOAT
	,OSCARG_TYPE_STRING
	,OSCARG_TYPE_BLOB
	,OSCARG_TYPE_BUNDLE
	,OSCARG_TYPE_OUTOFBOUND
} OSCArgType;


// container
class OSCArg {
public:
	OSCArg() {}
	virtual ~OSCArg() {};
	virtual OSCArgType getType() { return OSCARG_TYPE_NONE; }
	virtual string getTypeName() { return "none"; };
};

// int32
class OSCArgInt32 : public OSCArg {
public:
	OSCArgInt32(int32_t v):value(v) {}
	~OSCArgInt32()					{}
	
	virtual OSCArgType 	getType()		{ return OSCARG_TYPE_INT32; } 
	virtual string 		getTypeName()	{ return "int32"; 			} 
	int32_t 			get() const 	{ return value; 			}
	void 				set(int32_t v) 	{ value = v;				}			
	
private: 
	int32_t value;
};


// float
class OSCArgFloat : public OSCArg {
public:
	OSCArgFloat(float v):value(v) {}
	~OSCArgFloat(){}
	
	virtual OSCArgType 	getType()		{ return OSCARG_TYPE_FLOAT; } 
	virtual string 		getTypeName()	{ return "float"; 			} 
	float	 			get() const 	{ return value; 			}
	void 				set(float v) 	{ value = v;				}			
	
private: 
	float value;
};

// string
class OSCArgString : public OSCArg {
public:
	OSCArgString(string v):value(v) {}
	OSCArgString(){}
	
	virtual OSCArgType 	getType()		{ return OSCARG_TYPE_STRING; } 
	virtual string 		getTypeName()	{ return "string"; 			} 
	string	 			get() const 	{ return value; 			}
	void 				set(string v) 	{ value = v;				}			
	
private: 
	string value;
};

#endif