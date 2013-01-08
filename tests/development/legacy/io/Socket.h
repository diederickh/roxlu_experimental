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
 *
 *
 * UDP:
 * ------
 * This socket class can be used to UDP sockets too. This example shows
 * how to create a UDP socket which broadcasts some info on a specific
 * port (and broadcast ip): https://gist.github.com/731676c66291c6ff3785
 * 
 * When using UDP sockets, use "receiveFrom()" and "sendTo()"
 * receiveFrom() will fill the Socket& you pass as first parameter 
 * with information about the client who was sending some info.
 * 
 * You can use this returned/set Socket with sendTo. 
 * 
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
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

namespace roxlu {

	bool	 rx_get_broadcast_address(char* ip, char* netmask, char* buffer, size_t size);

class Socket {
public:
	Socket(int socketType = SOCK_STREAM, int socketProto = IPPROTO_TCP);
	~Socket();
	bool create(); // only call when you need to setup some socket options e.g. before binding
	bool connect(const char* ip, unsigned short int port, int timeout = -1);
	bool bind(const char* ip, unsigned short int port);
	bool listen(int maxQueue);
	bool accept(Socket& client);
	int read(char* buf, int count, int timeout = -1); // -2 = client disconnected (if possible to detect, else -1),  -1 = error while reading, 0 = got timeout (when timeout is used), 0 > = number of bytes read
	int send(const char* buf, int count); 
	int receiveFrom(Socket& client, char* buf, size_t size);  // receive from UDP socket
	int sendTo(Socket& client, const char* buf, size_t size); // send to UDP socket
	void close();
	void setPort(unsigned short int port); // set sockaddr_in port (usefull UDP)
	void setIP(const char* ip);  // set sockaddr_in IP (usefull for UDP)
	void setBroadcastIP(); // send to the broadcast IP
	bool setBlocking(bool flag);
	bool setNoDelay(); // on linux small packets are send in groups by default.
	bool setKeepAlive(); // http://tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/#whatis, set this if you want be notified by network disconnections
	bool setNoSigPipe();
	bool setReUseAddress();
	bool setBroadcast();
	bool isBlocking();
	bool isValid();

private:
	bool is_blocking;
	int socket_type; // protocol type: SOCK_STREAM, SOCK_DGRAM
	int socket_proto; // defaults to 0 (0 = use default protocol for protocol_type)

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
