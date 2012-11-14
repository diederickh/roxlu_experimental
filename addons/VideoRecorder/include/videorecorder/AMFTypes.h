#ifndef ROXLU_AMF_TYPESH
#define ROXLU_AMF_TYPESH

#include <string>
#include <iostream>
#include <vector>
#include <string>

#define AMF0_TYPE_NUMBER 0x00
#define AMF0_TYPE_BOOLEAN 0x01
#define AMF0_TYPE_STRING 0x02
#define AMF0_TYPE_OBJECT 0x03
#define AMF0_TYPE_MOVIECLIP 0x04 
#define AMF0_TYPE_NULL 0x05
#define AMF0_TYPE_UNDEFINED 0x06
#define AMF0_TYPE_REFERENCE 0x07
#define AMF0_TYPE_ECMA_ARRAY 0x08
#define AMF0_TYPE_OBJECT_END 0x09
#define AMF0_TYPE_STRICT_ARRAY 0x0A
#define AMF0_TYPE_DATE 0x0B
#define AMF0_TYPE_LONG_STRING 0x0C
#define AMF0_TYPE_UNSUPPORTED 0x0D
#define AMF0_TYPE_RECORDSET 0x0E
#define AMF0_TYPE_XML 0x0F
#define AMF0_TYPE_TYPED_OBJECT 0x10 

#define AMF_TYPE_NOT_SET 0x99

// AMF Types
// -------------------------------------------------------
struct AMFType {
   AMFType(int type);
	virtual ~AMFType();
	virtual void print();
	int getType();
	int type;
};

struct AMFContainer : public AMFType{
	AMFContainer();
	AMFContainer(int type);
	~AMFContainer();
	void push_back(AMFType* type);
	void deleteElements();
	std::vector<AMFType*>::iterator find(const std::string& name);
	void print();
	std::vector<AMFType*> elements;
};


struct AMFNumber : public AMFType {
	AMFNumber(double value);
	double getValue() { return value; }
	void print();
	double value;
};

struct AMFString : public AMFType {
	AMFString(const std::string& str, bool writeTag = false); // writeTag = write the tag marker
	~AMFString();
	void print();
	std::string getValue();
	std::string value;
	bool write_tag;
};

struct AMFBoolean : public AMFType {
	AMFBoolean(bool value);
	void print();
	bool getValue();
	bool value;
};

struct AMFObject : public AMFType {
	AMFObject();
};

struct AMFMovieClip : public AMFType {
	AMFMovieClip();
};

struct AMFNull : public AMFType {
	AMFNull();
};

struct AMFUndefined : public AMFType {
	AMFUndefined();
};

struct AMFReference : public AMFType {
	AMFReference();
};

// Last element is an empty string (end marker)
struct AMFEcmaArray : public AMFContainer {
	AMFEcmaArray();
	void operator()(const std::string& name, AMFType* el);
};

struct AMFObjectEnd : public AMFType {
	AMFObjectEnd();
	void print();
};

struct AMFStrictArray : public AMFType {
	AMFStrictArray();
};

struct AMFDate : public AMFType {
	AMFDate();
};

struct AMFLongString : public AMFType {
	AMFLongString();
};

struct AMFUnsupported : public AMFType {
	AMFUnsupported();
};

struct AMFRecordSet : public AMFType {
	AMFRecordSet();
};

struct AMFXML : public AMFType
 {
	AMFXML();
};

struct AMFTypedObject : public AMFType {
	AMFTypedObject();
};

#endif
