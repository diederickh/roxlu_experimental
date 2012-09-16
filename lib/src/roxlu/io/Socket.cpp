#include <roxlu/io/Socket.h>

namespace roxlu {
Socket::Socket() 
	:is_blocking(true) // by default
{  
#ifdef _WIN32
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if(result != 0) {
		printf("Error: cannot initialize WinSock.\n");
		WSACleanup();
	}
	else {
		create();
	}
#elif __APPLE__
	create();
#endif
}

Socket::~Socket() {
}

bool Socket::create() {
#ifdef _WIN32
	printf("Creating socking...\n");
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) {
		printf("Error: cannot create socket, error code: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	// When the socket is closed and reopened, we need to reset state.
	setBlocking(is_blocking);
	
#elif __APPLE__
	printf("@todo, when the socket is closed and reopened we need to reset the blocking state. \n");

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		printf("Error: cannot create socket.\n");
		return false;
	}
	printf("@todo test is reseting blocking state works\n");
	setBlocking(is_blocking); 
#endif

	return true;
}

bool Socket::isValid() {
	int so_type;
	socklen_t len = sizeof(so_type);
#ifdef _WIN32
	int ret = getsockopt(sock, SOL_SOCKET, SO_TYPE, (char*)&so_type, &len);
#elif __APPLE__
	int ret = getsockopt(sock, SOL_SOCKET, SO_TYPE, (void*)&so_type, &len);
#endif

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
	bool was_blocking = is_blocking;
	if(timeout > 0 && is_blocking) {
		setBlocking(false);
	}
	
	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_addr.s_addr = inet_addr(ip);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);

	int result = ::connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in));
	if(timeout > 0 ) {
		if(result == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK) {
				fd_set write_set, err_set;
				FD_ZERO(&write_set);
				FD_ZERO(&err_set);
				FD_SET(sock, &write_set);
				FD_SET(sock, &err_set);

				TIMEVAL tv;
				tv.tv_sec = timeout;
				tv.tv_usec = 0;

				result = select(0, NULL, &write_set, &err_set, &tv);

				// Go back into blocking mode if needed
				if(was_blocking && !is_blocking) {
					setBlocking(true);
				}
				
				if(result == 0) {
					printf("Socket: connect(), Connect timeout.\n");
					return false;
				}
				else {
					if(FD_ISSET(sock, &write_set)) {
						printf("Socket: conntect() connected!\n");
					}
					else {
						printf("Socket: connect() unhandled situation...\n");
					}
				}
			}
			else {
				if(err == WSAEISCONN) {
					return true;
				}
				printf("Error: connect() failed to connect. (%d)\n", err);
				//WSACleanup();
				//close();
				return false;
			}
		}
	}
	else {
		// Connecting in blocking mode
		if(result == SOCKET_ERROR) {
			printf("Error: cannot connect to server.\n");
			WSACleanup();
			close();
			return false;
		}
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
		if(was_blocking && !is_blocking) {
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
		setBlocking(false);
	}

#ifdef _WIN32
	if(timeout > 0) {
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);
		TIMEVAL tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		int result = select(0, &fdset, NULL, NULL, &tv);

		if(result == SOCKET_ERROR) {
			printf("Error: read(), error with select: %d\n", WSAGetLastError());
			return -1;
		}
		
		// reset blocking state.
		if(was_blocking && !is_blocking) {
			setBlocking(true);
		}

		if(result == -1) {
			printf("Error: error with select(), cannot read().\n");
			return result;
		}
		else if(result == 0) {
			printf("Error: timeout while trying to read.\n");
			return 0;
		}
		else {
			int result = recv(sock, buf, count, 0);
			if(result == SOCKET_ERROR) {
				int err = WSAGetLastError();
				if(err == WSAEWOULDBLOCK) {
					return 0;
				}
				else {
					printf("Error: read(), error while reading, got error code: %d\n", err);
					return -1;
				}
			}
			else if(result > 0) {
				return result;
			}
			else {
				int err = WSAGetLastError();
				printf("Error: trying to read but it looks like the socket is closed (@todo maybe add a member to keep track of connection state). %d \n", err);
				return -1;
			}
		}
	}
	else {
		// receive w/o timeut
		int result = recv(sock, buf, count, 0);
		if(result == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if(err == WSAEWOULDBLOCK) {
				return 0;
			}
			else if(err == WSAECONNRESET) {
				printf("Error: read(), connected closed by remote host. %d.\n", err);
				close();
				return -2; // on Windows -2 means that the client was disconnected!
			}
			else if(err == WSAENOTCONN) {
				printf("Error: read(), socket is not connected. %d\n", err);
				return -2;
			}
			else {
				printf("Error: read(), error while reading, got error code: %d\n", err);
				return -1;
			}
		}
		else if(result > 0) {
			return result;
		}
		else {
			int err = WSAGetLastError();
			printf("Error: trying to read but it looks like the socket is closed (@todo maybe add a member to keep track of connection state). %d \n", err);
			return -1;
		}
	}
	
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
		printf("Error while sending... @todo check the error, and probably remove WSACleanup(), errcode: %d\n", WSAGetLastError());
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
	
	u_long flags = (mustBlock) ? 0 : 1;
	int result = ioctlsocket(sock, FIONBIO, &flags);

	if(result != 0) {
		int err = WSAGetLastError();
		printf("Error: cannot set the socket into non-blocking mode, error: %d.\n", err);
		return false;
	}
	is_blocking = mustBlock;
	return true;
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
