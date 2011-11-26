#ifndef ROXLU_IOBUFFERH
#define ROXLU_IOBUFFERH

#include <inttypes.h>
#include <string>
#include <algorithm>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

// Based on CRTMP server source code.
namespace roxlu {

#define GET_IB_POINTER(x) 				 ((uint8_t *)((x).buffer + (x).consumed))
#define GET_AVAILABLE_BYTES_COUNT(x)     ((x).published - (x).consumed)

class IOBuffer {
public:	
	uint8_t* buffer;
	uint32_t	size;
	uint32_t	published;
	uint32_t	consumed;
	uint32_t	min_chunk_size;
	
public:
	IOBuffer();
	virtual ~IOBuffer();
	
	// setup
	void setup();
	void setup(uint32_t expectedSize);
		
	bool loadFromFile(string path); // no datapath (needs to be clean)	
	bool saveToFile(string path);
		
	// moving the read head
	bool reuse(uint32_t numBytes); // opposite of ignore
	bool ignore(uint32_t numBytes);
	void reset(); // resets consume and publish positions to start.
	void resetConsumed();
	void resetStored();
		
	// store data
	bool storeBytes(const char* someData, const uint32_t numBytes);
	bool storeBytes(const uint8_t* someData, const uint32_t numBytes);
	void storeByte(uint8_t byte);
	void storeUI8(uint8_t byte);
	void storeUI16(uint16_t data);
	void storeUI32(uint32_t data);
	void storeString(string data);
	void storeRepeat(uint8_t byte, uint32_t numBytes); // rename!
	void storeBuffer(IOBuffer& other); // copies only stored data
	int storeBuffer(IOBuffer& other, uint32_t numBytes);
	void storeFloat(float data);
	void storeBool(bool data);
	void storeUI16LE(uint16_t data);
	void storeUI32LE(uint32_t data);
	void storeFloatLE(float data);
	void storeDoubleLE(double data);
	void storeUI16BE(uint16_t data);
	void storeUI32BE(uint32_t data);
	void storeUI64BE(uint64_t data);
	void storeDoubleBE(double data);
	void storeFloatBE(float data);
	
	void storeStringWithSizeBE(string data); // adds a uint16_t with the size of the string 
	void storeStringWithSizeLE(string data); // adds a uint16_t with the size of the string 
	string consumeStringWithSizeBE();
	string consumeStringWithSizeLE();
		
	// get the number of bytes published.
	uint32_t getNumBytesStored();
	void setNumBytesStored(uint32_t numBytes); // for direct manipulation of buffer.
	void addNumBytesStored(uint32_t numBytes); 
	bool hasBytesToRead();
	void addNumBytesConsumed(uint32_t numByes);
	uint32_t getMostNumberOfBytesWeCanConsume(uint32_t tryToRead); // namin gwill change..
					
	// reading data back from buffer moving read head
	int consumeUntil(uint8_t until, string& found);
	int consumeUntil(string until, string& found);
	int consumeBytes(uint8_t* buff, uint32_t numBytes);

	string consumeString(uint32_t upToNumBytes);


	uint8_t consumeByte();
	uint8_t  consumeUI8();
	uint16_t consumeUI16();
	uint32_t consumeUI32();
	uint64_t consumeUI64();
	float	consumeFloat();
	bool	consumeBool();

	int8_t  consumeI8();
	int16_t consumeI16();
	int32_t consumeI32();
	int64_t consumeI64();
	double consumeDouble();
	


	int16_t consumeI16LE();
	int32_t consumeI32LE();
	int64_t consumeI64LE();
	double consumeDoubleLE();
	uint16_t consumeUI16LE();
	uint32_t consumeUI32LE();
	uint64_t consumeUI64LE();
	float consumeFloatLE();
	
	uint16_t consumeUI16BE();
	uint32_t consumeUI32BE();
	uint64_t consumeUI64BE();
	int16_t consumeI16BE();
	int32_t consumeI32BE();
	int64_t consumeI64BE();
	double consumeDoubleBE();	
	
	// operators
	uint8_t& operator[](uint32_t index) const;
	
	// helpers
	inline uint8_t getAt(uint8_t position);
	void printHex(uint32_t start = 0, uint32_t end = 0);
	void printDoubleAsHex(double toPrint);
	void printUI16AsHex(uint16_t toPrint);
	void recycle();
	bool ensureSize(uint32_t expectedSize);
	void cleanup();
	bool moveData();
	void setMinChunkSize(uint32_t minSize);
	uint32_t getMinChunkSize();
	uint8_t* getPtr();
	uint8_t* getStorePtr();
	uint8_t* getConsumePtr();
};


inline uint8_t IOBuffer::getAt(uint8_t position) {
	return buffer[position];
}

} // roxlu
#endif