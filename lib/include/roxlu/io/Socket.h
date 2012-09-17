#ifndef ROXLU_IO_SOCKETH
#define ROXLU_IO_SOCKETH

/**
 * Cross platform, simple socket wrapper.
 *
 *
 * WINDOWS 
 * --------
 * On windows you could get "redefinition" errors. If so this means that 
 * you've included windows.h before this Socket.h file. A solution is to 
 * add "_WINSOCKAPI_" in your "C/C++ > Preprocessor" settings.
 */

#ifdef _WIN32 
 
#include <winsock2.h>
#include <ws2tcpip.h>
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
	bool setKeepAlive(); // http://tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/#whatis, set this if you want be notified by network disconnections
	bool isBlocking();
	bool isValid();
private:
	bool is_blocking;
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
