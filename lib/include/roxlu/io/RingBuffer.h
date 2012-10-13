#ifndef ROXLU_IO_RINGBUFFERH
#define ROXLU_IO_RINGBUFFERH

class RingBuffer {
public:
	RingBuffer(size_t capacity);
	~RingBuffer();
	size_t write(const char* data, size_t bytes);
	size_t read(char* data, size_t bytes);
	size_t drain(size_t bytes);
	size_t size();
	char* getReadPtr();

private:
	size_t read_index;
	size_t write_index;
	size_t capacity; // total amount of bytes we could store in the buffer
	size_t bytes_stored;  // total bytes currently stored
	char* buffer;
};

inline size_t RingBuffer::size() {
	return bytes_stored;
}

inline char* RingBuffer::getReadPtr() {
	return buffer + read_index;
}
#endif
