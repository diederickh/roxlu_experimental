#ifndef ROXLU_IOBUFFERH
#define ROXLU_IOBUFFERH
                           
#include <roxlu/core/platform/Platform.h>
//#include <cstdint>
#include <string>
#include <algorithm>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

// Based on CRTMP server source code.
namespace roxlu {

#define GET_IB_POINTER(x) 				((rx_uint8 *)((x).buffer + (x).consumed))
#define GET_AVAILABLE_BYTES_COUNT(x)   	((x).published - (x).consumed)

class IOBuffer {
public:	
	rx_uint8*	buffer;
	rx_uint32	size;
	rx_uint32	published;
	rx_uint32	consumed;
	rx_uint32	min_chunk_size;
	
public:
	IOBuffer();
	virtual ~IOBuffer();
	
	// setup
	void 		setup();
	void 		setup(rx_uint32 expectedSize);
		
	bool 		loadFromFile(string path); // no datapath (needs to be clean)	
	bool 		saveToFile(string path);
		
	// moving the read head
	bool 		reuse(rx_uint32 numBytes); 
	bool 		ignore(rx_uint32 numBytes);
	void 		reset(); 
	void 		resetConsumed();
	void 		resetStored();
		
	// get the number of bytes published.
	rx_uint32 	getNumBytesStored();
	void 		setNumBytesStored(rx_uint32 numBytes); 
	void 		addNumBytesStored(rx_uint32 numBytes); 
	bool 		hasBytesToRead();
	void 		addNumBytesConsumed(rx_uint32 numByes);
	rx_uint32 	getMostNumberOfBytesWeCanConsume(rx_uint32 tryToRead);

		
	// store: system byte order
	bool 		storeBytes(const char* someData, const rx_uint32 numBytes);
	bool 		storeBytes(const rx_uint8* someData, const rx_uint32 numBytes);
	void 		storeByte(rx_uint8 byte);
	void 		storeUI8(rx_uint8 byte);
	void 		storeUI16(rx_uint16 data);
	void 		storeUI32(rx_uint32 data);
	void 		storeString(string data);
	void 		storeRepeat(rx_uint8 byte, rx_uint32 numBytes); 
	void 		storeBuffer(IOBuffer& other); 
	int  		storeBuffer(IOBuffer& other, rx_uint32 numBytes);
	void 		storeFloat(float data);
	void 		storeBool(bool data);
	
	// store: little endian
	void 		storeUI16LE(rx_uint16 data);
	void 		storeUI32LE(rx_uint32 data);
	void 		storeFloatLE(float data);
	void 		storeDoubleLE(double data);
	void 		storeStringWithSizeLE(string data); 
	
	// store: big endian (network byte order)
	void 		storeUI16BE(rx_uint16 data);
	void 		storeUI32BE(rx_uint32 data);
	void 		storeUI64BE(rx_uint64 data);
	void 		storeDoubleBE(double data);
	void 		storeFloatBE(float data);
	void 		storeStringWithSizeBE(string data); 

	// consume: system byte order
	rx_uint8 	consumeByte();
	rx_uint8 	consumeUI8();
	rx_uint16	consumeUI16();
	rx_uint32 	consumeUI32();
	rx_uint64 	consumeUI64();
	float		consumeFloat();
	bool		consumeBool();
	rx_int8  	consumeI8();
	rx_int16 	consumeI16();
	rx_int32 	consumeI32();
	rx_int64 	consumeI64();
	double 		consumeDouble();
	int 		consumeUntil(rx_uint8 until, string& found);
	int 		consumeUntil(string until, string& found);
	int 		consumeBytes(rx_uint8* buff, rx_uint32 numBytes);
	string 		consumeString(rx_uint32 upToNumBytes);

	// consume: little endian
	rx_int16 	consumeI16LE();
	rx_int32 	consumeI32LE();
	rx_int64 	consumeI64LE();
	double 		consumeDoubleLE();
	rx_uint16 	consumeUI16LE();
	rx_uint32 	consumeUI32LE();
	rx_uint64 	consumeUI64LE();
	float 		consumeFloatLE();
	string 		consumeStringWithSizeLE();
		
	// consume: big endian
	rx_uint16 	consumeUI16BE();
	rx_uint32 	consumeUI32BE();
	rx_uint64 	consumeUI64BE();
	rx_int16 	consumeI16BE();
	rx_int32 	consumeI32BE();
	rx_int64 	consumeI64BE();
	double 		consumeDoubleBE();	
	string 		consumeStringWithSizeBE();
	
	// operators
	rx_uint8& operator[](rx_uint32 index) const;
	
	// helpers
	inline rx_uint8 	getAt(rx_uint8 position);
	void 			printHex(rx_uint32 start = 0, rx_uint32 end = 0);
	void 			printDoubleAsHex(double toPrint);
	void 			printUI16AsHex(rx_uint16 toPrint);
	void 			recycle();
	bool 			ensureSize(rx_uint32 expectedSize);
	void 			cleanup();
	bool 			moveData();
	void 			setMinChunkSize(rx_uint32 minSize);
	rx_uint32 		getMinChunkSize();
	rx_uint8* 		getPtr();
	rx_uint8* 		getStorePtr();
	rx_uint8* 		getConsumePtr();
};


inline rx_uint8 IOBuffer::getAt(rx_uint8 position) {
	return buffer[position];
}

} // roxlu
#endif