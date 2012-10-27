#include <twitter/Buffer.h>

Buffer::Buffer()
  :dx(0)
{
}

void Buffer::addBytes(const char* data, size_t len) {
  std::copy(data, data+len, std::back_inserter(buffer));
}

void Buffer::print() {
  for(int i = 0; i < buffer.size(); ++i) {
    printf("%c", buffer[i]);
  }
}
