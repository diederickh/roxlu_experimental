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

#define SWAP_UINT16(x)                          \
	((rx_uint16)((((x) & 0x00FFU) << 8) |         \
               (((x) & 0xFF00U) >> 8) )) 

// swap 3 big endian stored bytes
#define SWAP_UINT24(x)                          \
	((rx_uint32)((((x) & 0x00FF0000U) >> 16 ) |   \
               (((x) & 0x000000FFU) << 16 ) |   \
               (((x) & 0x0000FF00)) ))


/*
  #define SWAP_UINT24(x) \
	((rx_uint32)((((x) & 0xFF000000U) >> 24 ) | \
  (((x) & 0x00FF0000U) >> 8  ) | \
  (((x) & 0x0000FF00U) << 8  )  ))
*/
#define SWAP_UINT32(x)                          \
	((rx_uint32)((((x) & 0x000000FFU) << 24) |    \
               (((x) & 0x0000FF00U) << 8)   |   \
               (((x) & 0x00FF0000U) >> 8)   |   \
               (((x) & 0xFF000000U) >> 24)))

#define SWAP_UINT64(x)                                    \
	((rx_uint64)(                                           \
    ((((rx_uint64)(x)) & 0xFF00000000000000LL) >> 56) |   \
    ((((rx_uint64)(x)) & 0x00FF000000000000LL) >> 40) |   \
    ((((rx_uint64)(x)) & 0x0000FF0000000000LL) >> 24) |   \
    ((((rx_uint64)(x)) & 0x000000FF00000000LL) >> 8)  |   \
    ((((rx_uint64)(x)) & 0x00000000FF000000LL) << 8)  |   \
    ((((rx_uint64)(x)) & 0x0000000000FF0000LL) << 24) |   \
    ((((rx_uint64)(x)) & 0x000000000000FF00LL) << 40) |   \
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
  Buffer(char* data, size_t nbytes);
  ~Buffer();

	bool loadFile(const char* file);
	bool saveFile(const char* file);
	void drain(rx_uint32 num);
	void clear();
	rx_uint32 getReadIndex();
	size_t size();
  char* ptr(); // returns a char* to the beginning of the buffer 
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
  void putFloat(float f);
	
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
  float getFloat();

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

  // Stream operators
  Buffer& operator<<(const char* cstr);
  Buffer& operator<<(std::string str);
  Buffer& operator<<(int i);
  Buffer& operator>>(std::string& str);
  Buffer& operator>>(int& i);
  Buffer& operator<<(bool b);
  Buffer& operator>>(bool& b);
  Buffer& operator<<(float f);
  Buffer& operator>>(float& f);

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

inline char* Buffer::ptr() {
	return (char*)&data[0];
}

inline rx_uint8* Buffer::getReadPtr() {
	return &data[read_dx];
}

inline void Buffer::clear() {
	data.clear();
	read_dx = 0;
}

inline void Buffer::getBytes(char* result, rx_uint32 num) {
	return getBytes((rx_uint8*)result, num);
}

inline void Buffer::putBytes(char* data, int num) {
	putBytes((rx_uint8*)data, num);
}

inline Buffer& Buffer::operator<<(const char* cstr) {
  *this << (std::string)cstr;
  return *this;
}

inline Buffer& Buffer::operator<<(std::string str) {
  putBigEndianU32(str.size());
  if(str.size()) {
    putBytes((char*)str.c_str(), str.size());
  }
  return *this;
}

inline Buffer& Buffer::operator>>(std::string& str) {
  rx_uint32 size = getBigEndianU32();
  if(size) { 
    str.assign((char*)getReadPtr(), size);
    drain(size);
  }
  return *this;
}

inline Buffer& Buffer::operator<<(int i) {
  putBigEndianS32(i);
  return *this;
}

inline Buffer& Buffer::operator>>(int& i) {
  i = getBigEndianS32();
  return *this;
}

inline Buffer& Buffer::operator>>(bool& b) {
  b = getU8();  
  return *this;
}

inline Buffer& Buffer::operator<<(bool b) {
  putByte((rx_uint8)(b) ? 1 : 0);
  return *this;
}

inline Buffer& Buffer::operator<<(float f) {
  putFloat(f);
  return *this;
}

inline Buffer& Buffer::operator>>(float& f) {
  f = getFloat();
  return *this;
}

/*

  The BUFFERIZE macro allows you easily pack and unpack 
  a struct into a Buffer which you then can send over the 
  network and read back.

  ````c++

 struct Person {
  Person():age(0),score(0.0),alive(false) {}
  int age;
  std::string name;
  float score;
  bool alive;
  int result;

  void print() {
    printf("age: %d,  ", age);
    printf("name: %s, ", name.c_str());
    printf("score: %f, ", score);
    printf("alive: %d ", alive);
    printf("result: %d", result);
    printf("\n");
  }

  // here we generate the `pack(Buffer& b)` and `unpack(Buffer& b)` methods
  BUFFERIZE(age, name, score, result, alive);

 };

  Person person_a;
  person_a.name = "roxlu";
  person_a.age = 100;
  person_a.score = 200.5;
  person_a.alive = true;
  person_a.result = 145;

  // PACK
  Buffer buffered;
  person_a.pack(buffered);


  // UNPACK
  Person unpacked_person;
  unpacked_person.unpack(buffered);
 
  ````

*/
#define BUFFERIZE(...)                              \
  void pack(Buffer& b) {                            \
    make_bufferizer_define(__VA_ARGS__).pack(b);    \
  }                                                 \
                                                    \
  void unpack(Buffer& b) {                          \
    make_bufferizer_define(__VA_ARGS__).unpack(b);  \
  }

template<typename a0 = void,typename a1 = void,typename a2 = void,typename a3 = void,typename a4 = void,typename a5 = void,typename a6 = void,typename a7 = void,typename a8 = void,typename a9 = void,typename a10 = void,typename a11 = void,typename a12 = void,typename a13 = void,typename a14 = void,typename a15 = void,typename a16 = void,typename a17 = void,typename a18 = void,typename a19 = void> struct bufferizer_define;

template< typename a0>
struct bufferizer_define<a0> { 
 bufferizer_define(a0& arg0):m0(arg0){}
  void pack(Buffer& b) { b << m0; }
  void unpack(Buffer& b) { b >> m0; }
  a0& m0;
};

template<typename a0>
bufferizer_define<a0> make_bufferizer_define(a0& arg0) {
  return bufferizer_define<a0>(arg0);
}

template< typename a0,typename a1>
  struct bufferizer_define<a0,a1> { 
 bufferizer_define(a0& arg0,a1& arg1):m0(arg0),m1(arg1){}
  void pack(Buffer& b) { b << m0 << m1; }
  void unpack(Buffer& b) { b >> m0 >> m1; }
  a0& m0; a1& m1;
};

template<typename a0,typename a1>
  bufferizer_define<a0,a1> make_bufferizer_define(a0& arg0,a1& arg1) {
  return bufferizer_define<a0,a1>(arg0,arg1);
}

template< typename a0,typename a1,typename a2>
  struct bufferizer_define<a0,a1,a2> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2):m0(arg0),m1(arg1),m2(arg2){}
  void pack(Buffer& b) { b << m0 << m1 << m2; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2; }
  a0& m0; a1& m1; a2& m2;
};

template<typename a0,typename a1,typename a2>
  bufferizer_define<a0,a1,a2> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2) {
  return bufferizer_define<a0,a1,a2>(arg0,arg1,arg2);
}

template< typename a0,typename a1,typename a2,typename a3>
  struct bufferizer_define<a0,a1,a2,a3> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3):m0(arg0),m1(arg1),m2(arg2),m3(arg3){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3; }
  a0& m0; a1& m1; a2& m2; a3& m3;
};

template<typename a0,typename a1,typename a2,typename a3>
  bufferizer_define<a0,a1,a2,a3> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3) {
  return bufferizer_define<a0,a1,a2,a3>(arg0,arg1,arg2,arg3);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4>
  struct bufferizer_define<a0,a1,a2,a3,a4> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4>
  bufferizer_define<a0,a1,a2,a3,a4> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4) {
  return bufferizer_define<a0,a1,a2,a3,a4>(arg0,arg1,arg2,arg3,arg4);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5>
  bufferizer_define<a0,a1,a2,a3,a4,a5> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5>(arg0,arg1,arg2,arg3,arg4,arg5);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6>(arg0,arg1,arg2,arg3,arg4,arg5,arg6);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13),m14(arg14){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13 << m14; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13 >> m14; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13; a14& m14;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13),m14(arg14),m15(arg15){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13 << m14 << m15; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13 >> m14 >> m15; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13; a14& m14; a15& m15;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13),m14(arg14),m15(arg15),m16(arg16){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13 << m14 << m15 << m16; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13 >> m14 >> m15 >> m16; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13; a14& m14; a15& m15; a16& m16;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16,typename a17>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16,a17& arg17):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13),m14(arg14),m15(arg15),m16(arg16),m17(arg17){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13 << m14 << m15 << m16 << m17; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13 >> m14 >> m15 >> m16 >> m17; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13; a14& m14; a15& m15; a16& m16; a17& m17;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16,typename a17>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16,a17& arg17) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17);
}

template< typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16,typename a17,typename a18>
  struct bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18> { 
 bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16,a17& arg17,a18& arg18):m0(arg0),m1(arg1),m2(arg2),m3(arg3),m4(arg4),m5(arg5),m6(arg6),m7(arg7),m8(arg8),m9(arg9),m10(arg10),m11(arg11),m12(arg12),m13(arg13),m14(arg14),m15(arg15),m16(arg16),m17(arg17),m18(arg18){}
  void pack(Buffer& b) { b << m0 << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8 << m9 << m10 << m11 << m12 << m13 << m14 << m15 << m16 << m17 << m18; }
  void unpack(Buffer& b) { b >> m0 >> m1 >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8 >> m9 >> m10 >> m11 >> m12 >> m13 >> m14 >> m15 >> m16 >> m17 >> m18; }
  a0& m0; a1& m1; a2& m2; a3& m3; a4& m4; a5& m5; a6& m6; a7& m7; a8& m8; a9& m9; a10& m10; a11& m11; a12& m12; a13& m13; a14& m14; a15& m15; a16& m16; a17& m17; a18& m18;
};

template<typename a0,typename a1,typename a2,typename a3,typename a4,typename a5,typename a6,typename a7,typename a8,typename a9,typename a10,typename a11,typename a12,typename a13,typename a14,typename a15,typename a16,typename a17,typename a18>
  bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18> make_bufferizer_define(a0& arg0,a1& arg1,a2& arg2,a3& arg3,a4& arg4,a5& arg5,a6& arg6,a7& arg7,a8& arg8,a9& arg9,a10& arg10,a11& arg11,a12& arg12,a13& arg13,a14& arg14,a15& arg15,a16& arg16,a17& arg17,a18& arg18) {
  return bufferizer_define<a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18>(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17,arg18);
}

#endif

