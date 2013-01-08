#ifndef ROXLU_DICTIONARYH
#define ROXLU_DICTIONARYH

//#include <roxlu/core/platform/Platform.h>
//#include <cstdint>
// @todo remove this file from the lib
#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <iostream>
#include <vector>

#include <roxlu/io/IOBuffer.h>

//#include "IOBuffer.h"

#define IN_MAP(m,k)	((bool)((m).find((k))!=(m).end()))
#define VAR_INDEX_VALUE "__index__value__"

using namespace std;

namespace roxlu {
	enum DictionaryType {
		 D_NULL			=	0
		,D_UNDEFINED 	=	1
		,D_BOOL			= 	2
		
		,D_INT8			=	3
		,D_INT16		=	4
		,D_INT32		=	5
		,D_INT64		=	6

		,D_UINT8		=	7
		,D_UINT16		=	8
		,D_UINT32		=	9
		,D_UINT64		=	10
		,D_NUMERIC		=	11
		
		,D_DOUBLE		= 	12
		,D_STRING		=	13
		,D_MAP			= 	14
		,D_BYTEARRAY	=	15
	};

	struct DictionaryMap;

	class Dictionary {
	public:
		// constructors.
		Dictionary();
		~Dictionary();
		Dictionary(IOBuffer& buffer);
		Dictionary(const Dictionary& val);
		Dictionary(const bool& val);
		Dictionary(const double& val);
		Dictionary(const string& val);
		Dictionary(const char* val);
		
		Dictionary(const rx_int8 &val);
		Dictionary(const rx_int16 &val);
		Dictionary(const rx_int32 &val);
		Dictionary(const rx_int64 &val);
		Dictionary(const rx_uint8 &val);
		Dictionary(const rx_uint16 &val);
		Dictionary(const rx_uint32 &val);
		Dictionary(const rx_uint64 &val);
		
		// assignment operators.
		Dictionary& operator=(const Dictionary& val);
		Dictionary& operator=(const bool &val);
		Dictionary& operator=(const double &val);
		Dictionary& operator=(const string &val);
		Dictionary& operator=(const char* chars);
		
		Dictionary& operator=(const rx_int8 &val);
		Dictionary& operator=(const rx_int16 &val);
		Dictionary& operator=(const rx_int32 &val);
		Dictionary& operator=(const rx_int64 &val);
		
		Dictionary& operator=(const rx_uint8 &val);
		Dictionary& operator=(const rx_uint16 &val);
		Dictionary& operator=(const rx_uint32 &val);
		Dictionary& operator=(const rx_uint64 &val);
		
		Dictionary& operator[](const string& key);
		Dictionary& operator[](const char* key);
		Dictionary& operator[](const rx_uint32& key);
		Dictionary& operator[](Dictionary& key);
		
		bool operator==(Dictionary other); // @todo pass by ref?
		bool operator!=(Dictionary other);
		bool operator==(DictionaryType type);
		bool operator!=(DictionaryType type);
		
		friend std::ostream& operator<<(std::ostream& os, roxlu::Dictionary& dict);
		
		// retrieve as a forced type
		rx_uint8  getAsUInt8();
		rx_uint16 getAsUInt16();
		rx_uint32 getAsUInt32();
		rx_uint64 getAsUint64();
		
		rx_int8 	getAsInt8();
		rx_int16  getAsInt16();
		rx_int32  getAsInt32();
		rx_int64  getAsInt64();
		
		// type casting
		operator bool();
		operator rx_int8();
		operator rx_int16();
		operator rx_int32();
		operator rx_int64();
		operator rx_uint8();
		operator rx_uint16();
		operator rx_uint32();
		operator rx_uint64();
		operator double();
		operator string();
		
		string toJSON();
		bool fromJSON(string json);
		
		bool toBinary(IOBuffer& buffer);
		bool fromBinary(IOBuffer& buffer); 
	
		string toXML();
		
		bool isNumeric();
		bool isBoolean();
		
		// string functions
		static void replaceString(string& target, string search, string replacement);
		static string stringToLower(string str);
		
		// destructor related
		void reset(bool isUndefined = false);
		
		// array related
		void isByteArray(bool makeByteArray);
		bool isByteArray();
		bool isArray();
		void isArray(bool makeArray);
		rx_uint32 getMapSize();
		rx_uint32 getMapDenseSize();
		void pushToArray(Dictionary dict);

		bool hasKey(const string key);
		void removeKey(const string& key);
		void removeAt(const rx_uint32 index);

		DictionaryType type;
		typedef map<string, Dictionary>::iterator iterator;
		map<string, Dictionary>::iterator begin();
		map<string, Dictionary>::iterator end();
		
	private:	
		static bool binaryDeserialize(IOBuffer& buffer, Dictionary& result);
		
		bool jsonSerialize(string &result);
		static bool jsonDeserialize(string& json, Dictionary& result, rx_uint32& start);
		static void jsonEscape(string &value);
		static void jsonUnEscape(string& value);
		static bool jsonReadWhiteSpace(string& raw, rx_uint32& start);		
		static bool jsonReadDelimiter(string& raw, rx_uint32& start, char& c);
		static bool jsonReadString(string& raw, Dictionary& result, rx_uint32& start);
		static bool jsonReadNumber(string& raw, Dictionary& result, rx_uint32& start);
		static bool jsonReadObject(string& raw, Dictionary& result, rx_uint32& start);
		static bool jsonReadArray(string& raw, Dictionary& result, rx_uint32& start);
		static bool jsonReadBool(string& raw, Dictionary& result, rx_uint32& start, string wanted);
		static bool jsonReadNull(string& raw, Dictionary& result, rx_uint32& start);
		
		string toString(string name="", rx_uint32 indent = 0);										
		
		union {
			bool 		b;
			rx_int8 	i8;
			rx_int16	i16;
			rx_int32	i32;
			rx_int64	i64;
			rx_uint8	ui8;
			rx_uint16	ui16;
			rx_uint32	ui32;
			rx_uint64	ui64;
			double		d;
			string*		s;
			DictionaryMap* m;
		} value;
		
		void copyFrom(const Dictionary& other);
	};
	
	inline std::ostream& operator<<(std::ostream& os, Dictionary& dict)  {
		os << dict.toJSON();
		return os ;
	}
}


#endif