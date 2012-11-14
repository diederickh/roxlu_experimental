#ifndef ROXLU_FLV_AMFH
#define ROXLU_FLV_AMFH

#include <string>
#include <roxlu/io/Buffer.h>
#include <videorecorder/AMFTypes.h>

class AMF {
public:
	void putType(Buffer& buffer, AMFType* v);
	void putNumberAMF0(Buffer& buffer, AMFNumber v);
	void putBooleanAMF0(Buffer& buffer, AMFBoolean v);
	void putFalseAMF0(Buffer& buffer);
	void putTrueAMF0(Buffer& buffer);
	void putStringAMF0(Buffer& buffer, AMFString v);
	void putUndefinedAMF0(Buffer& buffer);
	void putNullAMF0(Buffer& buffer);
	void putEcmaArrayAMF0(Buffer& buffer, AMFEcmaArray v);

	bool parseBuffer(Buffer& buffer, AMFContainer& result);
	AMFType* parseType(Buffer& buffer);

	AMFNumber* getNumberAMF0(Buffer& buffer);
	AMFBoolean* getBooleanAMF0(Buffer& buffer);
	AMFString* getStringAMF0(Buffer& buffer);
	AMFLongString* getLongStringAMF0(Buffer& buffer);
	AMFEcmaArray* getEcmaArrayAMF0(Buffer& buffer);
	AMFObjectEnd* getObjectEndAMF0(Buffer& buffer);
};

#endif
