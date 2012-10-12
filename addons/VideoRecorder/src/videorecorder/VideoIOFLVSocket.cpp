#include <videorecorder/VideoIOFLVSocket.h>

void VideoIOFLVSocket::connect(const char* ip, unsigned short int port) {
	if(sock.connect(ip, port, 10)) {
		printf("# Connected.\n");
	}
	else {
		printf("# ERROR: cannot connect.\n");
	}
}

void VideoIOFLVSocket::flush(Buffer& buffer) {
	int num_bytes = buffer.getNumBytesLeftToRead();

	int s = sock.send((const char*)buffer.getReadPtr(), num_bytes);
	printf("FLUSH!, from: %d, to: %d, bytes send: %d\n", buffer.getReadIndex(), num_bytes, s);
	if(s != num_bytes) {
		printf("# ERROR OH OH!!\n");
	}
	buffer.drain(num_bytes);
	buffer.clear();
}
