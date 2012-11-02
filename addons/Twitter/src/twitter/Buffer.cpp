#include <twitter/Buffer.h>

Buffer::Buffer()
  :dx(0)
{
}

size_t Buffer::getNumBytesToRead() {
  return getSize() - dx;
}

void Buffer::addBytes(const char* data, size_t len) {
  std::copy(data, data+len, std::back_inserter(buffer));
}

void Buffer::flushReadBytes() {
  buffer.erase(buffer.begin(), buffer.begin()+dx);
  dx = 0;
}

void Buffer::print() {
  for(int i = dx; i < buffer.size(); ++i) {
    printf("%c", buffer[i]);
  }
}
