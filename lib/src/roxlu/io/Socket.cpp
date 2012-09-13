#include <roxlu/io/Socket.h>

namespace roxlu {
Socket::Socket() {  
#ifdef _WIN32
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if(result != 0) {
		printf("Error: cannot initialize WinSock.\n");
		WSACleanup();
	}
#endif
}

Socket::~Socket() {
}

bool Socket::connect(const char* ip, unsigned short int port) {
#ifdef _WIN32
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) {
		printf("Error: cannot create socket, error code: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_addr.s_addr = inet_addr(ip);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);

	int result = ::connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in));
	if(result == SOCKET_ERROR) {
		printf("Error: cannot connect to server.\n");
		WSACleanup();
		close();
		return false;
	}

#elif __APPLE__
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		printf("Error: cannot create socket.\n");
		return false;
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr(ip);	
	addr_in.sin_port = htons(port);
	
	int status = ::connect(sock, (struct sockaddr *)&addr_in, sizeof(addr_in));
	printf("Status: %d\n", status);
	if(status == 0) {
		printf("Connected to %s\n", ip);
	}
	else {
		close();
	}
#endif

	return true;
}

// 0: disconnect,  < 0 error, > 0 number of bytes read.
int Socket::read(char* buf, int count) {
#ifdef _WIN32
	int result = recv(sock, buf, count, 0);
	if(result == 0) {
		printf("Error: trying to read but socket is closed (@todo maybe add a member to keep track of connection state).\n");
		return 0;
	}
	else if(result < 0) {
		printf("Error: cannot read from socket, error: %d\n",WSAGetLastError()); 
		return result;
	}
	return result;
#elif __APPLE__
	int status = ::read(sock, buf, count);
	if(status < 0) {
		printf("Error: cannot read from socket. Probably disconnected. %d\n", status);
	}
	return status;
#endif
}

// returns -1 on error, else the number of bytes sent.
int Socket::send(const char* buf, int count) {
#ifdef _WIN32
	int status = ::send(sock, buf, count, 0);
	if(status == SOCKET_ERROR) {
		close();
		WSACleanup();
		return -1; 
	}
	return status;
#elif __APPLE__
	int status = ::write(sock, buf, count);
	if(status < 0) {
		printf("Error: cannot write on socket. Probably disconnected. %d\n", status);
		return -1;
	}
	return status;
#endif
}

void Socket::close() {
#ifdef _WIN32
	closesocket(sock);
#elif __APPLE__
	::close(sock);
#endif
}

} // roxlu
