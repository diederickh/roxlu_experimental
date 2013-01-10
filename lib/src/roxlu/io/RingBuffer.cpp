#include <roxlu/io/RingBuffer.h>
#include <algorithm>

RingBuffer::RingBuffer(size_t capacity) 
  :capacity(capacity)
  ,write_index(0)
  ,read_index(0)
  ,bytes_stored(0)
  ,buffer(NULL)

{
  if(capacity > 0) {
    buffer = new char[capacity];
    memset(buffer, 0, capacity);
  }
}

RingBuffer::~RingBuffer() {
  if(buffer != NULL) {
    delete[] buffer;
  }
}

void RingBuffer::resize(size_t bytes) {
  if(buffer == NULL) {
    buffer = new char[bytes];
    memset(buffer, 0, capacity);
    capacity = bytes;
  }
  else {
    if(bytes < capacity) {
      return; // we do not shrink
    }

    size_t to_grow = capacity - bytes;
    if(to_grow <= 0) {
      return;
    }
    char* tmp_buffer = new char[bytes];
    memset(tmp_buffer, 0, bytes);

    if(size() > 0) {
      memcpy(tmp_buffer, buffer, size());
    }
    delete[] buffer;
    buffer = tmp_buffer;
    capacity = bytes;
  }
}

size_t RingBuffer::write(const char* data, size_t bytes) {
  if(capacity == 0) {
    resize(1024*1024);
  }
  if(bytes == 0) {
    return 0;
  }
  size_t cap = capacity;
  size_t to_write = std::min<size_t>(bytes, cap - bytes_stored);

  if(to_write <= (cap - write_index)) { 
    // we have enough space till the end of the buffer
    memcpy(buffer + write_index, data, to_write);
    write_index += to_write;
    if(write_index == capacity) {
      write_index = 0; 
    }
  }
  else {
    // there is not enough space at the end.. just fill up and start at the begin
    size_t size1 = cap - write_index; 
    memcpy(buffer + write_index, data, size1);
    size_t size2 = to_write - size1;
    memcpy(buffer, data + size1, size2);
    write_index = size2;
  }
	 
  bytes_stored += to_write;
  return to_write;
}

size_t RingBuffer::read(char* data, size_t bytes) {
  if(bytes == 0) {
    return 0;
  }

  size_t cap = capacity;
  size_t to_read = std::min<size_t>(bytes, bytes_stored);

  if(to_read <= cap - read_index) {
    // we won't touch the end of the buffer..
    memcpy(data, buffer + read_index, to_read);
    read_index += to_read;
    if(read_index == cap) {
      read_index = 0;
    }
  }
  else {
    // we read something till the end, then start at the begin
    size_t size1 = cap - read_index;
    memcpy(data, buffer + read_index, size1);
    size_t size2 = to_read - size1;
    memcpy(data + size1, buffer, size2);
    read_index = size2;
  }
  bytes_stored -= to_read;
  return to_read;
}

// Drains some bytes w/o copying read bytes to another buffer.
size_t RingBuffer::drain(size_t bytes) {
  if(bytes == 0) {
    return 0;
  }
  size_t cap = capacity;
  size_t to_read = std::min<size_t>(bytes, bytes_stored);

  if(to_read <= cap - read_index) {
    read_index += to_read;
    if(read_index == cap) {
      read_index = 0;
    }
  }
  else {
    size_t size1 = cap - read_index;
    size_t size2 = to_read - size1;
    read_index = size2;
  }
  bytes_stored -= to_read;
  return to_read;
}
