#include <roxlu/io/Buffer.h>

Buffer::Buffer()
	:read_dx(0)
{
}

Buffer::Buffer(char* raw, size_t nbytes) {
  read_dx = 0;
  putBytes(raw, nbytes);
}

Buffer::~Buffer() {
	//	printf("~Buffer()\n");
}

bool Buffer::loadFile(const char* filename) {
	std::ifstream ifs(filename, std::ios::binary | std::ios::in | std::ios::ate) ;
	if(!ifs.is_open()){
		printf("Cannot load: %s\n", filename);
		return false;
	}
	rx_uint32 size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	char* tmp_buf = new char[size];
	ifs.read(tmp_buf, size);
	ifs.close();
	
	putBytes((rx_uint8*)tmp_buf, size);
	delete[] tmp_buf;
	return true;
}

bool Buffer::saveFile(const char* filename) {
	std::ofstream ofs(filename, std::ios::binary | std::ios::out);
	if(!ofs.is_open()) {
		printf("Cannot write: %s\n", filename);
		return false;
	}
	ofs.write((char*)getPtr(), size());
	if(!ofs) {
		printf("Something went wrong while writing to FLV file.\n");
		return false;
	}
	return true;
}


void Buffer::drain(rx_uint32 num) {
	read_dx += num;
}

// Store in native byte order.
void Buffer::putByte(rx_uint8 b) {
	data.push_back(b);
}

void Buffer::putBytes(rx_uint8* b, int num) {
	data.insert(data.end(), b, b+num);
}

void Buffer::putBytes(rx_uint8* b, int num, int pos) {
	for(int i = pos, j = 0; i < (pos+num); ++i, ++j) {
		data[i] = b[j];
	}
}

void Buffer::putReversed(rx_uint8* b, int num) {
	for(int i = num-1; i >= 0; --i) {
		//		printf("Put: %d\n", i);
		putByte(*(b+i));
	}
}

void Buffer::rewriteByte(rx_uint32 position, rx_uint8 b) {
	data[position] = b;
}

void Buffer::rewriteReversed(rx_uint32 position, rx_uint8* b, rx_uint32 num) {
	int start = position + num;
	int end = position;
	rx_uint32 c = 0;
	for(int i = start, c = 0; i > position; --i, ++c) {
		//printf("> %02X (%d)\n", *(b+c), position+c);
		//data[position+c] = *(b+c);
		data[i-1] = *(b+c);
	}
}

void Buffer::putString(const char* data, int num) {
	putBytes((rx_uint8*)data, num);
}

void Buffer::putU8(rx_uint8 b) {
	putByte(b);
}

void Buffer::putU16(rx_uint16 b) {
	putBytes((rx_uint8*)&b, 2);
}

void Buffer::putU32(rx_uint32 b) {
	putBytes((rx_uint8*)&b, 4);
}

void Buffer::putU64(rx_uint64 b) {
	putBytes((rx_uint8*)&b, 8);
}

void Buffer::putS8(rx_int8 b) {
	putBytes((rx_uint8*)&b, 1);
}

void Buffer::putS16(rx_int16 b) {
	putBytes((rx_uint8*)&b, 2);
}

void Buffer::putS32(rx_int32 b) {
	putBytes((rx_uint8*)&b, 4);
}

void Buffer::putS64(rx_int64 b) {
	putBytes((rx_uint8*)&b, 8);
}

void Buffer::putFloat(float f) {
  putBytes((rx_uint8*)&f, sizeof(f));
}

// Store in big endian byte order (convert if necessary)
void Buffer::putBigEndianU16(rx_uint16 b) {
	b = CONVERT_TO_BE_U16(b);
	putBytes((rx_uint8*)&b, 2);
}

void Buffer::putBigEndianU24(rx_uint32 b) {
	b = CONVERT_TO_BE_U24(b);
	putBytes((rx_uint8*)&b, 3);
}
void Buffer::putBigEndianU24(rx_uint32 b, rx_uint32 pos) {
	b = CONVERT_TO_BE_U24(b);
	putBytes((rx_uint8*)&b, 3, pos);
}

void Buffer::putBigEndianU32(rx_uint32 b) {
	b = CONVERT_TO_BE_U32(b);
	putBytes((rx_uint8*)&b, 4);
}

void Buffer::putBigEndianU64(rx_uint64 b) {
	b = CONVERT_TO_BE_U64(b);
	putBytes((rx_uint8*)&b, 8);
}

void Buffer::putBigEndianDouble(double b) {
	rx_uint64 tmp = 0;
	memcpy((char*)&tmp, (char*)&b, 8);
	tmp = CONVERT_TO_BE_U64(tmp);
	putBytes((rx_uint8*)&tmp, 8);
}

void Buffer::putBigEndianS16(rx_int16 b) {
	rx_uint32 c = 0;
	memcpy((char*)&c, (char*)&b, 2);
	putBigEndianU16(c);
}

void Buffer::putBigEndianS32(rx_int32 b) {
	rx_uint32 c = 0;
	memcpy((char*)&c, (char*)&b, 4);
	putBigEndianU32(c);
}

void Buffer::putBigEndianS64(rx_int64 b) {
	rx_uint64 c = 0;
	memcpy((char*)&c, (char*)&b, 8);
	putBigEndianU64(c);
}

// Store in little endian order (convert if necessary)
void Buffer::putLittleEndianU16(rx_uint16 b) {
	b = CONVERT_TO_LE_U16(b);
	putBytes((rx_uint8*)&b, 2);
}

void Buffer::putLittleEndianU32(rx_uint32 b) {
	b = CONVERT_TO_LE_U32(b);
	putBytes((rx_uint8*)&b, 4);
}

void Buffer::putLittleEndianU64(rx_uint64 b) {
	b = CONVERT_TO_LE_U64(b);
	putBytes((rx_uint8*)&b, 8);
}

void Buffer::putLittleEndianS16(rx_int16 b) {
	rx_uint16 c = 0;
	memcpy((char*)&c, (char*)&b, 2);
	putLittleEndianU16(c);
}

void Buffer::putLittleEndianS32(rx_int32 b) {
	rx_uint32 c = 0;
	memcpy((char*)&c, (char*)&b, 4);
	putLittleEndianU32(c);
}

void Buffer::putLittleEndianS64(rx_int64 b) {
	rx_uint64 c = 0;
	memcpy((char*)&c, (char*)&b, 8);
	putLittleEndianU64(b);
}

// Retrieving values from the buffer
// ----------------------------------------------------------------
void Buffer::getBytes(rx_uint8* result, rx_uint32 num) {
	memcpy((char*)result, (char*)&data[read_dx], num);
	read_dx += num;
}

rx_uint8 Buffer::getByte() {
	rx_uint8 b = data[read_dx];
	read_dx++;
	return b;
}

rx_uint8 Buffer::getU8() {
	return getByte();
}

rx_uint16 Buffer::getU16() {
	rx_uint16 r = 0;
	getBytes((rx_uint8*)&r, 2);
	return r;
}

rx_uint32 Buffer::getU24() {
	rx_uint32 r = 0;
	getBytes((rx_uint8*)&r, 3);
	return r;
}

rx_uint32 Buffer::getU32() {
	rx_uint32 r = 0;
	getBytes((rx_uint8*)&r, 4);
	return r;
}

rx_uint64 Buffer::getU64() {
	rx_uint64 r = 0;
	getBytes((rx_uint8*)&r, 8);
	return r;
}

rx_int8 Buffer::getS8() {
	read_dx++;
	return data[read_dx];
}

rx_int16 Buffer::getS16() {
	rx_uint16 c = 0;
	rx_int16 r = 0;
	getBytes((rx_uint8*)&c, 2);
	memcpy((char*)&r, (char*)&c, 2);
	return r;
}

rx_int32 Buffer::getS32() {
	rx_uint32 c = 0;
	rx_int32 r = 0;
	getBytes((rx_uint8*)&c, 4);
	memcpy((char*)&r, (char*)&c, 4);
	return r;
}

rx_int64 Buffer::getS64() {
	rx_uint64 c = 0;
	rx_int64 r = 0;
	getBytes((rx_uint8*)&c, 8);
	memcpy((char*)&r, (char*)&c, 8);
	return r;
}

float Buffer::getFloat() {
  float r = 0.0;
  getBytes((rx_uint8*)&r, sizeof(float));
  return r;
}

// Get in little endian byte order (convert if necessary)
rx_uint16 Buffer::getLittleEndianU16() {
	rx_uint16 r = getU16();
	r = CONVERT_FROM_LE_U16(r);
	return r;
}

rx_uint32 Buffer::getLittleEndianU32() {
	rx_uint32 r = getU32();
	r = CONVERT_FROM_LE_U32(r);
	return r;
}

rx_uint64 Buffer::getLittleEndianU64() {
	rx_uint64 r = getU64();
	r = CONVERT_FROM_LE_U64(r);
	return r;
}

/*
double Buffer::getLittleEndianDouble() {
	rx_uint64 r = getU64();
	return 0.0;
}
*/

rx_int16 Buffer::getLittleEndianS16() {
	rx_uint16 r = getLittleEndianU16();
	r = CONVERT_FROM_LE_U16(r);
	rx_int16 o = 0;
	memcpy((char*)&o, (char*)&r, 2);
	return o;
}

rx_int32 Buffer::getLittleEndianS32() {
	rx_uint32 r = getLittleEndianU32();
	r = CONVERT_FROM_LE_U32(r);
	rx_int32 o = 0;
	memcpy((char*)&o, (char*)&r, 4);
	return o;
}

rx_int64 Buffer::getLittleEndianS64() {
	rx_uint64 r = getLittleEndianU64();
	r = CONVERT_FROM_LE_U64(r);
	rx_int64 o = 0;
	memcpy((char*)&o, (char*)&r, 8);
	return o;
}

rx_uint16 Buffer::getBigEndianU16() {
	rx_uint16 r = getU16();
	r = CONVERT_FROM_BE_U16(r);
	return r;
}

rx_uint32 Buffer::getBigEndianU24() {
	rx_uint32 r = 0;
	memcpy((char*)&r, (char*)&data[read_dx], 4);
	r = CONVERT_FROM_BE_U24(r);
	read_dx += 3;
	return r;
}

rx_uint32 Buffer::getBigEndianU32() {
	rx_uint32 r = getU32();
	r = CONVERT_FROM_BE_U32(r);
	return r;
}

rx_uint64 Buffer::getBigEndianU64() {
	rx_uint64 r = getU64();
	r = CONVERT_FROM_BE_U64(r);
	return r;
}

double Buffer::getBigEndianDouble() {
	rx_uint64 r = getU64();
	r = CONVERT_FROM_BE_U64(r);
	double d = 0;
	memcpy((char*)&d, (char*)&r, 8);
	return d;
}


rx_int16 Buffer::getBigEndianS16() {
	rx_uint16 r = getU16();
	r = CONVERT_FROM_BE_U16(r);
	rx_int16 b = 0;
	memcpy((char*)&b, (char*)&r, 2);
	return b;
}

rx_int32 Buffer::getBigEndianS24() {
	rx_uint32 r = 0;
	memcpy((char*)&r, (char*)&data[read_dx], 4);
	r = CONVERT_FROM_BE_U24(r);
	read_dx += 3;

	rx_int32 v = 0;
	memcpy((char*)&v, (char*)&r, 4);
	return v;
}

rx_int32 Buffer::getBigEndianS32() {
	rx_uint32 r = getU32();
	r = CONVERT_FROM_BE_U32(r);
	rx_int32 b = 0;
	memcpy((char*)&b, (char*)&r, 4);
	return b;
}

rx_int64 Buffer::getBigEndianS64() {
	rx_uint64 r = getU64();
	r = CONVERT_FROM_BE_U64(r);
	rx_int64 b = 0;
	memcpy((char*)&b, (char*)&r, 8);
	return b;
}



// ----------------------------------------------------------------------
rx_uint8 Buffer::peekByte() {
	return data[read_dx];
}
// ----------------------------------------------------------------------

void Buffer::copyFrom(Buffer& other) {
	std::copy(other.data.begin(), other.data.end(), std::back_inserter(data));
}

void Buffer::copyTo(Buffer& other, int start, int numBytes) {
	std::copy(data.begin()+start, data.begin()+start+numBytes, std::back_inserter(other.data));
}

rx_uint32 Buffer::getNumBytesLeftToRead() {
	return data.size() - read_dx;
}

// Debug
void Buffer::print(int dx) {
	printf("%02X\n", data[dx]);
}

void Buffer::printReadIndex() {
	printf("%02X\n", data[read_dx]);
}

void Buffer::printList(int start, int end) {
	for(int i = start; i < end; ++i) {
		printf("%02d = %02X (%d)\n", i, data[i], data[i]);
	}
}

void Buffer::print() {
	print(0, data.size()-1);
}

void Buffer::print(int start, int end) {
	if(start >= data.size() || end > data.size()) {
		printf("Out of bound, %zu\n", data.size());
		return;
	}
	for(int i = start; i < end; ++i) {
		printf("%02X ", data[i]);
	}
	printf("\n");
}


void Buffer::printNext2() {
	print(read_dx, read_dx+2);
}

void Buffer::printNext4() {
	print(read_dx, read_dx+4);
}

void Buffer::printNext8() {
	print(read_dx, read_dx+8);
}

void Buffer::printU16(rx_uint16 u) {
	rx_uint8* p = (rx_uint8*)&u;
	printf("u16 = %02X %02X (low address first)\n", *(p), *(p+1));
}

void Buffer::printU32(rx_uint32 u) {
	rx_uint8* p = (rx_uint8*)&u;
	printf("u32 = %02X %02X %02X %02X (low address first)\n", *p, *(p+1), *(p+2), *(p+3));
}

void Buffer::printU64(rx_uint64 u) {
	rx_uint8* p = (rx_uint8*)&u;
	printf("u64 = %02X %02X %02X %02X %02X %02X %02X %02X (low address first)\n", 
			 *(p+0),*(p+1),*(p+2),*(p+3),*(p+4),*(p+5),*(p+6),*(p+7)); 
}


void Buffer::printBits(rx_uint8 u) {
	std::bitset<8> bs(u);
	std::cout << bs << std::endl;
}

void Buffer::compareWith(Buffer& other, int start, int stop) {
	if(other.size() < (start+stop) || size() < (start+stop)) {
		printf("Buffer cannot compare; would go out of buffer range.\n");
		return;
	}

	for(int i = start; i <= stop; ++i) {
		rx_uint8 a = (*this)[i];
		rx_uint8 b = other[i];
		printf("%d) %02X - %02X %s\n", i, a, b, a == b ? " Y " : " NO ");
	}
}

