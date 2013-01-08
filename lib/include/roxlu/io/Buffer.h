#ifndef ROXLU_BUFFERH
#define ROXLU_BUFFERH
#include <bitset>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>


#include <roxlu/Roxlu.h>

/* When on a little endian machine swap bytes */
#define USE_LITTLE_ENDIAN
#ifdef USE_LITTLE_ENDIAN

#define SWAP_UINT16(x) \
	((rx_uint16)((((x) & 0x00FFU) << 8) | \
					(((x) & 0xFF00U) >> 8) )) 

// swap 3 big endian stored bytes
#define SWAP_UINT24(x) \
	((rx_uint32)((((x) & 0x00FF0000U) >> 16 ) | \
					(((x) & 0x000000FFU) << 16 ) | \
					(((x) & 0x0000FF00)) ))


/*
#define SWAP_UINT24(x) \
	((rx_uint32)((((x) & 0xFF000000U) >> 24 ) | \
					(((x) & 0x00FF0000U) >> 8  ) | \
					(((x) & 0x0000FF00U) << 8  )  ))
*/
#define SWAP_UINT32(x) \
	((rx_uint32)((((x) & 0x000000FFU) << 24) | \
					(((x) & 0x0000FF00U) << 8)   | \
					(((x) & 0x00FF0000U) >> 8)   | \
					(((x) & 0xFF000000U) >> 24)))

#define SWAP_UINT64(x) \
	((rx_uint64)( \
					((((rx_uint64)(x)) & 0xFF00000000000000LL) >> 56) | \
					((((rx_uint64)(x)) & 0x00FF000000000000LL) >> 40) | \
					((((rx_uint64)(x)) & 0x0000FF0000000000LL) >> 24) | \
					((((rx_uint64)(x)) & 0x000000FF00000000LL) >> 8)  | \
					((((rx_uint64)(x)) & 0x00000000FF000000LL) << 8)  | \
					((((rx_uint64)(x)) & 0x0000000000FF0000LL) << 24) | \
					((((rx_uint64)(x)) & 0x000000000000FF00LL) << 40) | \
					((((rx_uint64)(x)) & 0x00000000000000FFLL) << 56)  ))

#define CONVERT_TO_LE_U16(x) (x)
#define CONVERT_TO_LE_U32(x) (x)
#define CONVERT_TO_LE_U64(x) (x)

#define CONVERT_TO_BE_U16(x) SWAP_UINT16(x)
#define CONVERT_TO_BE_U24(x) SWAP_UINT24(x)
#define CONVERT_TO_BE_U32(x) SWAP_UINT32(x)
#define CONVERT_TO_BE_U64(x) SWAP_UINT64(x)

#define CONVERT_FROM_LE_U16(x) (x)
#define CONVERT_FROM_LE_U24(x) (x)
#define CONVERT_FROM_LE_U32(x) (x)
#define CONVERT_FROM_LE_U64(x) (x)

#define CONVERT_FROM_BE_U16(x) SWAP_UINT16(x)
#define CONVERT_FROM_BE_U24(x) SWAP_UINT24(x)
#define CONVERT_FROM_BE_U32(x) SWAP_UINT32(x)
#define CONVERT_FROM_BE_U64(x) SWAP_UINT64(x)

#else
#error "We have not yet implemented conversion functions for BIG ENDIAN"
#endif


class Buffer {
public:
	Buffer();
  	~Buffer();

	bool loadFile(const char* file);
	bool saveFile(const char* file);
	void drain(rx_uint32 num);
	void clear();
	rx_uint32 getReadIndex();
	size_t size();
	rx_uint8* getPtr(); // get pointer to beginning of buffer
	rx_uint8* getReadPtr(); // get pointer to current read position.
	rx_uint32 getNumBytesLeftToRead();

	// peek bytes (do not move read head)
	rx_uint8 peekByte();

	void rewriteByte(rx_uint32 position, rx_uint8 b);
	void rewriteReversed(rx_uint32 position, rx_uint8* b, rx_uint32 num);

	// store in native byte order
	void putByte(rx_uint8 b);
	void putBytes(char* b, int num);
	void putBytes(rx_uint8* b, int num);
	void putBytes(rx_uint8* b, int num, int pos);
	void putReversed(rx_uint8* b, int num);
	
	void putU8(rx_uint8 b);
	void putU16(rx_uint16 b);
	void putU32(rx_uint32 b);
	void putU64(rx_uint64 b);
	void putS8(rx_int8 b);
	void putS16(rx_int16 b);
	void putS32(rx_int32 b);
	void putS64(rx_int64 b);
	
	void putString(const char* data, int num);
	
	// store in big endian byte order (convert if necessary)
	void putBigEndianU16(rx_uint16 b);
	void putBigEndianU24(rx_uint32 b);
	void putBigEndianU24(rx_uint32 b, rx_uint32 pos); // rewrite buffer at pos
	void putBigEndianU32(rx_uint32 b);
	void putBigEndianU64(rx_uint64 b);
	void putBigEndianDouble(double b);

	void putBigEndianS16(rx_int16 b);
	void putBigEndianS32(rx_int32 b);
	void putBigEndianS64(rx_int64 b);

	// store in little endian byte order (convert if necessary)
	void putLittleEndianU16(rx_uint16 b);
	void putLittleEndianU32(rx_uint32 b);
	void putLittleEndianU64(rx_uint64 b);

	void putLittleEndianS16(rx_int16 b);
	void putLittleEndianS32(rx_int32 b);
	void putLittleEndianS64(rx_int64 b);

	// retrieve bytes in native order
	void getBytes(char* result, rx_uint32 num);
	void getBytes(rx_uint8* result, rx_uint32 num);
	rx_uint8 getByte();
	rx_uint8 getU8();
	rx_uint16 getU16();
	rx_uint32 getU24();
	rx_uint32 getU32();
	rx_uint64 getU64();
	rx_int8 getS8();
	rx_int16 getS16();
	rx_int32 getS32();
	rx_int64 getS64();

	// retrieve in little endian order (convert if necessary)
	rx_uint16 getLittleEndianU16();
	rx_uint32 getLittleEndianU32();
	rx_uint64 getLittleEndianU64();
	//double getLittleEndianDouble();

	rx_int16 getLittleEndianS16();
	rx_int32 getLittleEndianS32();
	rx_int64 getLittleEndianS64();
	
	rx_uint16 getBigEndianU16();
	rx_uint32 getBigEndianU24(); // for flash
	rx_uint32 getBigEndianU32();
	rx_uint64 getBigEndianU64();
   double getBigEndianDouble();

	rx_int16 getBigEndianS16();
	rx_int32 getBigEndianS24(); // for flash
	rx_int32 getBigEndianS32();
	rx_int64 getBigEndianS64();

	void copyFrom(Buffer& other);
	void copyTo(Buffer& other, int start, int end);
	
	rx_uint8 operator[](unsigned int dx);

	// Debug
	void print();
	void print(int dx);
	void print(int start, int stop);
	void printList(int start, int stop);
	void printU16(rx_uint16 v);
	void printU32(rx_uint32 v);	
	void printU64(rx_uint64 v);
	void printNext2();
	void printNext4();
	void printNext8();
	void printReadIndex();
	void printBits(rx_uint8 byte);
	void compareWith(Buffer& other, int start, int stop);

private:
	rx_uint32 read_dx;
	std::vector<rx_uint8> data;
};

inline rx_uint8 Buffer::operator[](const unsigned int dx) {
	return data[dx];
}

inline rx_uint32 Buffer::getReadIndex() {
	return read_dx;
}

inline size_t Buffer::size() {
	return data.size();
}

inline rx_uint8* Buffer::getPtr() {
	return &data[0];
}

inline rx_uint8* Buffer::getReadPtr() {
	return &data[read_dx];
}

inline void Buffer::clear() {
	data.clear();
	read_dx = 0;
	printf("----------------- CLEARED: %zu\n", data.size());
}

inline void Buffer::getBytes(char* result, rx_uint32 num) {
	return getBytes((rx_uint8*)result, num);
}

inline void Buffer::putBytes(char* data, int num) {
	putBytes((rx_uint8*)data, num);
}
#endif
