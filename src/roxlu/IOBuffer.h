#ifndef IOBUFFERH
#define IOBUFFERH

#include <inttypes.h>
#include <string>
#include <algorithm>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

// Based on CRTMP server source code.


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
		
	bool loadFromFile(string path);	
	bool saveToFile(string path);
		
	// moving the read head
	bool reuse(uint32_t numBytes); // opposite of ignore
	bool ignore(uint32_t numBytes);
	void reset(); // resets consume and publish positions to start.
		
	// store data
	bool storeBytes(const char* someData, const uint32_t numBytes);
	bool storeBytes(const uint8_t* someData, const uint32_t numBytes);
	void storeByte(uint8_t byte);
	void storeUI8(uint8_t byte);
	void storeUI16(uint16_t data);
	void storeUI32(uint32_t data);
	void storeRepeat(uint8_t byte, uint32_t numBytes); // rename!
	void storeBuffer(IOBuffer& other); // copies only stored data
	void storeBuffer(IOBuffer& other, uint32_t numBytes);

	void storeString(string data);
	void storeStringWithSize(string data); // adds a uint16_t with the size of the string 
	void storeBigEndianUI16(uint16_t data);
	void storeBigEndianUI32(uint32_t data);
	void storeBigEndianUI64(uint64_t data);
	void storeBigEndianDouble(double data);
	
	// get the number of bytes published.
	uint32_t getNumBytesStored();
	void setNumBytesStored(uint32_t numBytes); // for direct manipulation of buffer.
	bool hasBytesToRead();
					
	// reading data back from buffer moving read head
	int consumeUntil(uint8_t until, string& found);
	int consumeUntil(string until, string& found);
	void consumeBytes(uint8_t* buff, uint32_t numBytes);

	string consumeString(uint32_t upToNumBytes);
	string consumeStringWithSize();

	uint8_t consumeByte();
	uint8_t  consumeUI8();
	uint16_t consumeUI16();
	uint32_t consumeUI32();
	uint64_t consumeUI64();

	int8_t  consumeI8();
	int16_t consumeI16();
	int32_t consumeI32();
	int64_t consumeI64();
	double consumeDouble();
	
	// when you want to convert from BE -> LE
	uint16_t consumeBigEndianUI16();
	uint32_t consumeBigEndianUI32();
	uint64_t consumeBigEndianUI64();
	int16_t consumeBigEndianI16();
	int32_t consumeBigEndianI32();
	int64_t consumeBigEndianI64();
	double consumeBigEndianDouble();	
	
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
};


inline uint8_t IOBuffer::getAt(uint8_t position) {
	return buffer[position];
}
#endif