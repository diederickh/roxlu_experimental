//#include "IOBuffer.h"
//#include "Endianness.h"

#include <roxlu/io/IOBuffer.h>
#include <roxlu/io/Endianness.h>


#include <iostream>

using std::cout;
using std::endl;

namespace roxlu {

IOBuffer::IOBuffer() 
:buffer(NULL)
,size(0)
,published(0)
,consumed(0)
,min_chunk_size(5)
{
	setup(); // should we do this? or its up to the user (?)
}

IOBuffer::~IOBuffer() {
	cleanup();
}


bool IOBuffer::loadFromFile(string path) {
	ifstream ifs(path.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
	if(!ifs.is_open()) {
		printf("IOBuffer error: cannot read file\n");
		return false;
	}
	// get size and go back to start.
	rx_uint32 file_size = ifs.tellg();
 	ifs.seekg(0, std::ios::beg);
	
	// read bytes into buffer.
 	char* tmp_buffer = new char [file_size];
	ifs.read (tmp_buffer, file_size);
 	ifs.close();
	
	// and copy the buffer.
	storeBytes(tmp_buffer, file_size);
	delete[] tmp_buffer;

	return true;
}

// http://www.cplusplus.com/reference/iostream/ofstream/ofstream/
bool IOBuffer::saveToFile(string path) {
	ofstream ofs(path.c_str(), std::ios::out | std::ios::binary);
	if(!ofs.is_open()) {
		return false;
	}

	ofs.write((char*)buffer, published);
	if(!ofs.good()) {
		ofs.close();
		return false;
	}
	ofs.close();
	return true;
}


void IOBuffer::setup() {
	setup(min_chunk_size);	
}

void IOBuffer::setup(rx_uint32 expectedSize) {
	if( (buffer != NULL)
		|| (size != 0)
		|| (published != 0)
		|| (consumed != 0)
	)
	{
		printf("iobuffer error: buffer has been setup already\n");
		return;
	}
	
	ensureSize(expectedSize);
	//memset(buffer, 0, expectedSize);
}

bool IOBuffer::ensureSize(rx_uint32 expectedSize) {
	
	// 1. we have enough space
	if(published + expectedSize <= size) {
		return true;
	}
	
	// 2. not enough space, try to move it.
	moveData();
	
	// 3. check if we have enough space after moving..
	if(published + expectedSize <= size) {
		return true;
	}
	
	// 4. not enough space. 
	// first allocate at least 1/3 of what we have and no less then min_chunk_size
	if( (published + expectedSize - size) < (size / 3)) {
		expectedSize = size + size / 3 - published;
	}
	if(expectedSize < min_chunk_size) {
		expectedSize = min_chunk_size;
	}
	
	// 5. create a new buffer.
	rx_uint8* tmp_buffer = new rx_uint8[published + expectedSize];
	
	// 6. copy exising data to tmp buffer
	if(buffer != NULL) {
		memcpy(tmp_buffer, buffer, published);
		delete[] buffer;
	}
	buffer = tmp_buffer;
	
	// 7. update our size.
	size = published + expectedSize;
	return true;
}

rx_uint8* IOBuffer::getPtr() {
	return buffer;
}

rx_uint8* IOBuffer::getStorePtr() {
	return buffer+published;
}

rx_uint8* IOBuffer::getConsumePtr() {
	return buffer+consumed;
}

bool IOBuffer::moveData() {
	if(published - consumed <= consumed) {
		memcpy(buffer, buffer+consumed, published - consumed);
		published = published - consumed;
		consumed = 0;
	}
	return true;
}

void IOBuffer::cleanup() {
	if(buffer != NULL) {
		//printf("* need to free memory in iobuffer \n");
		//delete[] buffer;
		buffer = NULL;
	}	
	size = 0;
	consumed = 0;
	published = 0;
}

void IOBuffer::setMinChunkSize(rx_uint32 minSize) {
	min_chunk_size = minSize;
}

rx_uint32 IOBuffer::getMinChunkSize() {
	return min_chunk_size;
}

void IOBuffer::storeByte(rx_uint8 byte) {
	ensureSize(1);
	buffer[published] = byte;
	published++;
}

void IOBuffer::storeBool(bool data) {
	storeByte((rx_uint8)data);
}

void IOBuffer::storeFloat(float data) {
	ensureSize(4);
	memcpy(buffer+published, &data, 4);
	published +=4;
}

bool IOBuffer::storeBytes(const rx_uint8* someData, const rx_uint32 numBytes) {
	if(!ensureSize(numBytes)) {
		return false;
	}
	memcpy(buffer+published, someData, numBytes);
	published += numBytes;
	return true;
}

bool IOBuffer::storeBytes(const char* someData, const rx_uint32 numBytes) {
	if(!ensureSize(numBytes)) {
		return false;
	}
	memcpy(buffer+published, someData, numBytes);
	published += numBytes;
	return true;
}


void IOBuffer::storeRepeat(rx_uint8 byte, rx_uint32 numBytes) {
	ensureSize(numBytes);
	memset(buffer + published, byte, numBytes);
	published += numBytes;
}

void IOBuffer::storeUI8(rx_uint8 byte) {
	storeByte(byte);
}

void IOBuffer::storeUI16(rx_uint16 data) {
	ensureSize(16);
	memcpy(buffer+published, &data, 2);
	published += 2;
}

void IOBuffer::storeUI32(rx_uint32 data) {
	ensureSize(32);
	memcpy(buffer+published, &data, 4);
	published += 4;
}

void IOBuffer::storeUI16BE(rx_uint16 data) {
	ensureSize(16);
	data = ToBE16(data);
	memcpy(buffer+published, &data, 2);
	published += 2;
}

void IOBuffer::storeUI32BE(rx_uint32 data) {
	ensureSize(4);
	data = ToBE32(data);
	memcpy(buffer+published, &data, 4);
	published += 4;
}

void IOBuffer::storeUI64BE(rx_uint64 data) {
	ensureSize(8);
	data = ToBE64(data);
	memcpy(buffer+published, &data, 8);
	published += 8;
}

void IOBuffer::storeDoubleBE(double data) {
	ensureSize(8);
	rx_uint64 val = 0;
	memcpy(&val, &data, 8);
	val = ToBE64(val);
	memcpy(buffer+published, &val, 8);
	published += 8;
}

void IOBuffer::storeFloatBE(float data) {
	ensureSize(4);
	data = ToBE32(data);
	memcpy(buffer+published, &data, 4);
	published += 4;
}


void IOBuffer::storeUI16LE(rx_uint16 data) {
	ensureSize(2);
	data = ToLE16(data);
	memcpy(buffer+published, &data, 2);
	published += 2;
}

void IOBuffer::storeUI32LE(rx_uint32 data) {
	ensureSize(4);
	data = ToLE32(data);
	memcpy(buffer+published, &data, 4);
	published += 4;
}

void IOBuffer::storeDoubleLE(double data) {
	ensureSize(8);
	rx_uint64 val = 0;
	memcpy(&val, &data, 8);
	val = ToLE64(data);
	memcpy(buffer+published, &val, 8);
	published += 8;
}

void IOBuffer::storeFloatLE(float data) {
	ensureSize(4);
	data = ToLE32(data);
	memcpy(buffer+published, &data, 4);
	published += 4;
}

void IOBuffer::storeString(string data) {
	rx_uint32 len = (rx_uint32)data.length();
	ensureSize(len);
	memcpy(buffer+published, (rx_uint8 *)data.c_str(), len);
	published += len;
}

// stores a rx_uint16 (big endian) + string
void IOBuffer::storeStringWithSizeBE(string data) {
	storeUI16BE(data.size());
	storeString(data);
}

void IOBuffer::storeStringWithSizeLE(string data) {
	storeUI16LE(data.size());
	storeString(data);
}

// copy data from another buffer.
void IOBuffer::storeBuffer(IOBuffer& other) {
	storeBuffer(other, other.getNumBytesStored());	
}

int IOBuffer::storeBuffer(IOBuffer& other, rx_uint32 numBytes) {
	// check if we can read this many bytes from other buffer.
	numBytes = other.getMostNumberOfBytesWeCanConsume(numBytes);
	if(numBytes == 0) {
		return 0;
	}
	ensureSize(numBytes);
	memcpy(buffer+published, other.buffer+other.consumed, numBytes);
	published += numBytes;
	other.published += numBytes;
	return numBytes;
}

rx_uint32 IOBuffer::getMostNumberOfBytesWeCanConsume(rx_uint32 tryToRead) {
	int space = published - consumed;
	if(space <= 0) {
		return 0;
	}

	if(tryToRead > space) {
		tryToRead = space;
	}
	return tryToRead;
}


bool IOBuffer::ignore(rx_uint32 numBytes) {
	consumed += numBytes;
	recycle();	
	return true;
}
bool IOBuffer::reuse(rx_uint32 numBytes) {
	consumed -= numBytes;
	return true;
}

void IOBuffer::setNumBytesStored(rx_uint32 numBytes) {
	published = numBytes;
}

void IOBuffer::addNumBytesStored(rx_uint32 numBytes) {
	published += numBytes;
}

void IOBuffer::addNumBytesConsumed(rx_uint32 numBytes) {
	consumed += numBytes;
}

rx_uint32 IOBuffer::getNumBytesStored() {
	return published;
}



void IOBuffer::recycle() {
	if(consumed != published) {
		return;
	}
	consumed = 0;
	published = 0;
}

void IOBuffer::reset() {
	consumed = 0; 
	published = 0;
}
void IOBuffer::resetConsumed() {
	consumed = 0;
}
void IOBuffer::resetStored() {
	published = 0;
}

// Retrieve from buffer and return data
//------------------------------------------------------------------------------
int IOBuffer::consumeBytes(rx_uint8* buf, rx_uint32 numBytes) {
	rx_int32 left = (published - consumed);
	if(left < numBytes) {
		numBytes = left;
	}
	rx_int32 available = left - numBytes;

	if(available <= 0) {
		return 0;
	}

	if(available < numBytes) {
		numBytes = available;
	}
	memcpy(buf,buffer+consumed, numBytes);
	consumed += numBytes;
	return numBytes;
}

rx_uint8 IOBuffer::consumeByte() {
	return consumeUI8();
}

rx_uint8 IOBuffer::consumeUI8() {
	rx_uint8 val = 0;
	memcpy(&val, buffer+consumed, 1);
	consumed += 1;
	return val;
}

bool IOBuffer::consumeBool() {
	return (bool)consumeUI8();
}

rx_uint16 IOBuffer::consumeUI16() {
	rx_uint16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	consumed += 2;
	return val;
}

rx_uint32 IOBuffer::consumeUI24() {
	rx_uint32 out = 0;
	out = (buffer[consumed] << 16) | (buffer[consumed+1] << 8) | buffer[consumed+2];
	consumed += 3;
	return out;
}

rx_uint32 IOBuffer::consumeUI32() {
	rx_uint32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	consumed += 4;
	return val;
}

rx_uint64 IOBuffer::consumeUI64() {
	rx_uint64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	consumed += 8;
	return val;
}

rx_int8  IOBuffer::consumeI8() {
	rx_int8 val = 0;
	memcpy(&val, buffer+consumed, 1);
	consumed += 1;
	return val;
}

rx_int16 IOBuffer::consumeI16() {
	rx_int16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	consumed += 2;
	return val;
}

// experimental
rx_int32 IOBuffer::consumeI24() {
	rx_int32 out = 0;
	out = (buffer[consumed] << 16) | (buffer[consumed+1] << 8) | buffer[consumed+2];
	consumed += 3;
	return out;
}

rx_int32 IOBuffer::consumeI32() {
	rx_int32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	consumed += 4;
	return val;
}

rx_int64 IOBuffer::consumeI64() {
	rx_int64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	consumed += 8;
	return val;
}

float IOBuffer::consumeFloat() {
	float val = 0.0f;
	memcpy(&val, buffer+consumed, 4);
	consumed += 4;
	return val;
}

double IOBuffer::consumeDouble() {
	double val = 0;
	memcpy(&val, buffer+consumed, 8);
	consumed += 8;
	return val;
}

// when you assume the data is big endian, convert it to system 
// -----------------------------------------------------------------------------
rx_uint16 IOBuffer::consumeUI16BE() {
	rx_uint16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	consumed += 2;
	val = FromBE16(val);
	return val;
}

// experimental
rx_uint32 IOBuffer::consumeUI24BE() {
	rx_uint32 out = 0;

	#ifdef ROXLU_LITTLE_ENDIAN
		out = (buffer[consumed] << 16) | (buffer[consumed+1] << 8) | buffer[consumed+2];
		consumed += 3;
		return out;
	#else
	return out;
	#endif
	
}

rx_uint32 IOBuffer::consumeUI32BE() {
	rx_uint32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	consumed += 4;
	val = FromBE32(val);
	return val;
}

rx_uint64 IOBuffer::consumeUI64BE() {
	rx_uint64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	val = FromBE64(val);
	consumed += 8;
	return val;
}

double IOBuffer::consumeDoubleBE() {
	rx_uint64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	val = FromBE64(val);

	double d = 0.0;
	memcpy(&d, &val, 8);

	consumed += 8;
	return d;
}

rx_int16 IOBuffer::consumeI16BE() {
	rx_int16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	val = FromBE16(val);
	consumed += 2;
	return val;
}

rx_int32 IOBuffer::consumeI32BE() {
	rx_int32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	val = FromBE32(val);
	consumed += 4;
	return val;
}

// Little Endian
rx_uint16 IOBuffer::consumeUI16LE() {
	rx_uint16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	consumed += 2;
	val = FromLE16(val);
	return val;
}

rx_uint32 IOBuffer::consumeUI32LE() {
	rx_uint32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	consumed += 4;
	val = FromLE32(val);
	return val;
}

rx_uint64 IOBuffer::consumeUI64LE() {
	rx_uint64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	val = FromLE64(val);
	consumed += 8;
	return val;
}

double IOBuffer::consumeDoubleLE() {
	rx_uint64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	val = FromLE64(val);

	double d = 0.0;
	memcpy(&d, &val, 8);

	consumed += 8;
	return d;
}

rx_int16 IOBuffer::consumeI16LE() {
	rx_int16 val = 0;
	memcpy(&val, buffer+consumed, 2);
	val = FromLE16(val);
	consumed += 2;
	return val;
}

rx_int32 IOBuffer::consumeI32LE() {
	rx_int32 val = 0;
	memcpy(&val, buffer+consumed, 4);
	val = FromLE32(val);
	consumed += 4;
	return val;
}

rx_int64 IOBuffer::consumeI64LE() {
	rx_int64 val = 0;
	memcpy(&val, buffer+consumed, 8);
	val = FromLE64(val);
	consumed += 8;
	return val;
}

float IOBuffer::consumeFloatLE() {
	float val = 0.0f;
	memcpy(&val, buffer+consumed, 4);
	val = FromLE32(val);
	consumed += 4;
	return val;
}


// Searching for bytes in buffer and returning strings
// -----------------------------------------------------------------------------

// returns where we found the string, or 0 when not found.
int IOBuffer::consumeUntil(rx_uint8 until, string& found) {
	for(int i = consumed; i < published; ++i) {
		found.push_back((char)buffer[i]);
		if(buffer[i] == until) {
			consumed += i;
			return i;
		}
	}
	return 0;
}

// consume up to the given string, if not foudn return 0 else bytes consumed
int IOBuffer::consumeUntil(string until, string& found) {
	int search_size = until.size();
	const char* search = until.c_str();
	int k = 0;
	for(int i = consumed; i < published; ++i) {
		found.push_back((char)buffer[i]);
		if(buffer[i] == search[0]) {
			// out of buffer.
			if( (i + search_size) > published) {
				return 0;
			} 
			// check if next couple of chars is the search string.
			for(k = 0; k < search_size; ++k) {
				if(buffer[i+k] != search[k]) {
					break;
				}
			}
			// we found the search string, update consumed!
			if(k == search_size) {
				found.append((char*)buffer+(i+1), search_size-1);
				consumed += (i+search_size);
				return consumed;
			}
		}
	}
	found = "";
	return 0;
}	

string IOBuffer::consumeString(rx_uint32 upToNumBytes) {
	string str((char*)buffer+consumed, upToNumBytes);
	consumed += upToNumBytes;
	return str;
}


string IOBuffer::consumeStringWithSizeBE() {
	rx_uint16 num_bytes_in_string = consumeUI16BE();
	return consumeString(num_bytes_in_string);
}

string IOBuffer::consumeStringWithSizeLE() {
	rx_uint16 num_bytes_in_string = consumeUI16LE();
	return consumeString(num_bytes_in_string);
}

// operators
//------------------------------------------------------------------------------
rx_uint8& IOBuffer::operator[](rx_uint32 index) const {
	return buffer[index];
}

// helpers
//------------------------------------------------------------------------------
void IOBuffer::printHex(rx_uint32 start, rx_uint32 end) {
	if(end == 0) {
		end = published;
	}
	if(end > published) {
		end = published;
	}
	if(start > end) {
		start = end;		
	}
	int c = 0;
	for(int i = start,c=0; i < end; ++i,++c) {
		if(c > 20) {
			printf("\n");
			c = 0;
		}
		printf("%02X ", buffer[i]);
	}
	printf("\n");
}

void IOBuffer::printDoubleAsHex(double d) {
	rx_uint8 tmp_buf[8];
	memcpy(tmp_buf,&d,8);
	for(int i = 0; i < 8; ++i) {
		printf("%02X ", tmp_buf[i]);
	}
	printf("\n");
}

void IOBuffer::printUI16AsHex(rx_uint16 toPrint) {
	rx_uint8 tmp_buf[2];
	memcpy(tmp_buf, &toPrint, 2);
	printf("%02X %02X\n", tmp_buf[0], tmp_buf[1]);
}

bool IOBuffer::hasBytesToRead() {
	return consumed < published;
}

} // roxlu
