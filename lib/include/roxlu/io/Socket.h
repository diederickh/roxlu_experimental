#ifndef ROXLU_IO_SOCKETH
#define ROXLU_IO_SOCKETH

#ifdef _WIN32 

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif

namespace roxlu {

class Socket {
public:
	Socket();
	~Socket();
	bool connect(const char* ip, unsigned short int port, int timeout = -1);
	int read(char* buf, int count, int timeout = -1); // -2 = client disconnected (if possible to detect, else -1),  -1 = error while reading, 0 = got timeout (when timeout is used), 0 > = number of bytes read
	int send(const char* buf, int count); 
	void close();
	bool setBlocking(bool flag);
	bool setNoDelay(); // on linux small packets are send in groups by default.
	bool isBlocking();
private:
	bool is_blocking;
	bool isValid();
	bool create();

#ifdef _WIN32
	WSADATA wsa_data;
	SOCKET sock;
	struct sockaddr_in addr_in;
#else
	int sock;
	struct sockaddr_in addr_in;
#endif
};

inline bool Socket::isBlocking() {
	return is_blocking;
}

} // roxlu

#endif
