#include <roxlu/io/Socket.h>

namespace roxlu {
Socket::Socket() 
	:is_blocking(true) // by default
{  
}

Socket::~Socket() {
}

bool Socket::create() {
#ifdef _WIN32
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if(result != 0) {
		printf("Error: cannot initialize WinSock.\n");
		WSACleanup();
		return false;
	}
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) {
		printf("Error: cannot create socket, error code: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
#elif __APPLE__
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		printf("Error: cannot create socket.\n");
		return false;
	}
#endif

	return true;
}

bool Socket::isValid() {
	int so_type;
	socklen_t len = sizeof(so_type);
	int ret = getsockopt(sock, SOL_SOCKET, SO_TYPE, (void*)&so_type, &len);
	printf("RET: %d\n", ret);
	return ret == 0;
}

bool Socket::connect(const char* ip, unsigned short int port, int timeout) {


#ifdef _WIN32
	if(!isValid()) {
	   if(!create()) {
        	printf("Error: cannot create a valid socket.\n");
			return false;
      }
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
	if(!isValid()) {
		if(!create()) {
			printf("Error: cannot create a valid socket.\n");
			return false;
		}
	}
	bool was_blocking = is_blocking;
	if(timeout > 0) {
		printf("Set to non-blocking mode\n");
		setBlocking(false);
	}

	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr(ip);	
	addr_in.sin_port = htons(port);
	
	int status = ::connect(sock, (struct sockaddr *)&addr_in, sizeof(addr_in));
	if(timeout > 0) {
		// Try non blocking mode, with timeout.
		fd_set fdset;
		struct timeval tv;
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		int result = select(sock+1, NULL, &fdset, NULL, &tv);

		// Reset back to blocking state if necessary
		if(was_blocking && !is_blocking && timeout > 0 ) {
			setBlocking(true);
		}

		if(result == 1) {
			int so_error;
			socklen_t len = sizeof(so_error);
			getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
			if(so_error == 0) {
				printf("Socket is connected.\n");
			}
			else {
				printf("Error: cannot connect to %s:%d\n", ip, port);
				close();
				return false;
			}
		}
		else {
			printf("Error: Cannot select, error: %d, %s.\n", errno, strerror(errno));
			close();
			return false;
		}
	}
	else {
		// We tried to connect in blocking mode.
		if(status == 0) {
			printf("Connected to %s\n", ip);
		}
		else {
			close();
			return false;
		}
   }
#endif
	return true;
}

// 0: disconnect,  < 0 error, > 0 number of bytes read.
int Socket::read(char* buf, int count, int timeout) {
	bool was_blocking = is_blocking;
	if(timeout > 0 && is_blocking) {
		printf("READ ---------- set non blocking\n");
		setBlocking(false);
	}

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

	if(timeout > 0) {
		fd_set fdset;
		struct timeval tv;
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		int result = select(sock+1, &fdset, NULL, NULL, &tv);

		// reset blocking state
		if(was_blocking && !is_blocking && timeout > 0) {
			setBlocking(true);
		}

		if(result == -1) {
			printf("Error: cannot read. %d\n", result);
			return result;
		}
		else if(result == 0) {
			printf("Error: timed out while trying to read.\n");
			return 0;
		}
		else {
			int status = ::read(sock, buf, count);
			return status;
		}
	}
	else {
		int status = ::read(sock, buf, count);
		if(status > 0) {
			return status;
		}
		else if(is_blocking && status < 0) {
			printf("Error: cannot read from socket. Probably disconnected. %d, errno:%d, errms: %s\n", status, strerror(errno));
		}
		else if(!is_blocking && status == -1) { // non-blocking mode this means "EWOULDBLOCK"
			return 0;
		}
		else {
			return -1; // error in non-blocking mode
		}
		return status;
	}
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

bool Socket::setBlocking(bool mustBlock) {
  
#ifdef _WIN32
	printf("Error: setBlocking not implemented for win.\n");
	return false;
#elif __APPLE__
	int flags = fcntl(sock, F_GETFL, 0);
	if(flags < 0) {
		printf("Error: cannot get the current options for the socket when trying to change blocking.\n");
		return false;
	}
	flags = mustBlock ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	bool r = (fcntl(sock, F_SETFL, flags) == 0) ? true : false;
	is_blocking = r && mustBlock;
	return r;
#endif
}

bool Socket::setNoDelay() {
#ifdef _WIN32
	printf("Error: setNoDelay is not implemented for win!\n");
	return false;
#elif __APPLE__
	int nodelay = 1;
	int result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&nodelay, sizeof(int));
	return result == 0;
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
