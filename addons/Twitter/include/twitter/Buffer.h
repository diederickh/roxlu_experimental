#ifndef ROXLU_TWITTER_BUFFERH
#define ROXLU_TWITTER_BUFFERH

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
namespace roxlu { 
  namespace twitter {

class Buffer {
public:
  Buffer();
  void addBytes(const char* data, size_t len);
  const char* getPtr();
  const char* getReadPtr();
  size_t getSize();
  size_t getNumBytesToRead();
  size_t findNextByte(size_t start, char byte);
  void flushReadBytes();
  void print();

  char& operator[](const unsigned int dx);

  size_t dx;
  std::vector<char> buffer;
};

inline size_t Buffer::getSize() {
  return buffer.size();
}

inline const char* Buffer::getPtr() {
  return &buffer[0];
}

inline const char* Buffer::getReadPtr() {
  return getPtr() + dx;
}

inline char& Buffer::operator[](const unsigned int dx) {
  return buffer[dx];
}

inline size_t Buffer::findNextByte(size_t start, char byte) {
  size_t vpos = 0;
  for(size_t i = start; i < buffer.size(); ++i, ++vpos) {
    if(buffer[i] == ' ') {
      break;
    }
  }
  return vpos;
}
  } // roxlu
} // twitter
#endif
