#ifndef ROXLU_IO_RINGBUFFERH
#define ROXLU_IO_RINGBUFFERH

#include <cstddef>
#include <stdio.h>
#include <cstring>

class RingBuffer {
 public:
  RingBuffer(size_t capacity);
  ~RingBuffer();
  void resize(size_t bytes);
  size_t write(const char* data, size_t bytes);           /* write and automatically set the write index to the correct position, returns the number of bytes written */
  size_t read(char* data, size_t bytes);                  /* read and automatically change the read index (so freeing some bytes in the buffer) */
  size_t drain(size_t bytes);                             /* drain w/o copying the data.. basically same as read */
  size_t size();
  size_t getWriteIndex();
  size_t getReadIndex();
  char* getReadPtr();
  size_t getCapacity();
  void reset();                                           /* reset the buffer; you can start with a clean slate */
 private:
  size_t read_index;
  size_t write_index;
  size_t capacity;                                        /* total amount of bytes we could store in the buffer */
  size_t bytes_stored;                                    /* total bytes currently stored */
  char* buffer;
};

inline size_t RingBuffer::size() {
  return bytes_stored;
}

inline char* RingBuffer::getReadPtr() {
  return buffer + read_index;
}

inline size_t RingBuffer::getWriteIndex() {
  return write_index;
}

inline size_t RingBuffer::getReadIndex() {
  return read_index;
}

inline size_t RingBuffer::getCapacity() {
  return capacity;
}

inline void RingBuffer::reset() {
  write_index = 0;
  read_index = 0;
  bytes_stored = 0;

  if(buffer) {
    memset(buffer, 0x00, capacity);
  }
}
#endif
