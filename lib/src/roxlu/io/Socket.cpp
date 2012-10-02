#include <roxlu/io/Socket.h>

/*
@todo  The windows version closes() the socket but 
maybe we need to let the user close it. See read().
Also check what happens if all WSACleanup() routes
are executed. This might give problems when you want
to reconnect again.

@todo Make sure the use cases on mac and windows 
is the same, so returning -1 from read(), means 
e.g. client disconnected.

*/

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
		//WSACleanup();
		return false;
	}
	// When the socket is closed and reopened, we need to reset state.
	setBlocking(is_blocking);
	
#elif __APPLE__
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == -1) {
		printf("Error: cannot create socket.\n");
		return false;
	}
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

bool Socket::bind(const char* ip, unsigned short int port) {
#ifdef _WIN32
#elif __APPLE__
	if(!isValid()) {
		if(!create()) {
			printf("Error: cannot create valid socket to bind.\n");
			return false;
		}
	}
	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);  // bind to any network card
	addr_in.sin_port = htons(port);
	int result = ::bind(sock, (struct sockaddr*)&addr_in, sizeof(addr_in));

	if(result != 0) {
		printf("Error: could not bind socket on %s:%d, error: %s (%d)\n", ip, port, strerror(errno), errno);
		close();
		return false;
	}
	return true;
#endif
}

bool Socket::listen(int maxQueue) {
#ifdef _WIN32
#elif __APPLE__
	int result = ::listen(sock, maxQueue);
	if(result == -1) {
		printf("Error: cannot start to listen.\n");
		close();
		return false;
	}
	return true;
#endif
}

bool Socket::accept(Socket& client) {
#ifdef _WIN32
#elif __APPLE__
	Socket client_sock;
	memset(&client.addr_in, 0, sizeof(client.addr_in));
	socklen_t len = sizeof(client.addr_in);
	client.sock = ::accept(sock, (struct sockaddr*)&client.addr_in, &len);
	if(client.sock == -1) {
		printf("Error: cannot create client socket.\n");
		return false;
	}
	return true;
#endif
}

/**
 * Connect to an ip address.
 * 
 * @param const char* ip       The IP of the remote: "192.168.0.103"
 * @param short port           The port to connect to 
 * @param int timeout          When given, we go into nonblocking mode and use 
 *                             select() with a time out. Then we reset the blocking
 *                             state if necessary. 
 * 
 * @return boolean   true:     We're connected
 *                   false:    Disconnected/not able to connect
 * 
 */
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
			//WSACleanup();
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
		else if(result == EINPROGRESS) {
			// This is correct, a non-blocking socket returns EINPROGRESS until it can connect
			printf("Warning: got EINPROGRESS... waiting for other side to come alive.\n");
			return false;
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

/**
 * Read data from socket (w or w/o timeout)
 *
 * @return integer         Return values are slightly different per mode:
 *
 *                         With timeout
 *                         ------------
 *                         0   = we timed out
 *                         < 0 = error
 *                         > 0 = number of bytes rececied
 *
 *                         W/o timeout
 *                         -----------
 *                         0 >= : number of bytes received. When we're in non-blocking
 *                                mode we will return lots of 0s when there is no data
 *                                on the socket
 *                         < 0  : error
 */ 
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
			printf("Notice: timeout while trying to read.\n");
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
			printf("Error: cannot read, error returned by select() %d\n", result);
			return result;
		}
		else if(result == 0) {
			printf("Notice: timed out while trying to read.\n");
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
			printf("Error: cannot read from socket. Probably disconnected. %d, errno:%d, errms: %s\n", status, errno, strerror(errno));
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
		//WSACleanup();
		return -1; 
	}
	return status;
#elif __APPLE__
	int status = ::write(sock, buf, count);
	//int status = ::send(sock, buf, count, 0);
	//printf("SEND status: %d\n", status);
	if(status < 0) {
		printf("Error: cannot write on socket. Probably disconnected. %s (%d), socket: %d, status: %d\n", strerror(errno), errno, sock, status);
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

bool Socket::setNoSigPipe() {
#ifdef _WIN32
	printf("@todo implmenet setNoSigPipe on windows\n");
#elif __APPLE__
	int set = 1;
	int result = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
	return result == 0;
#endif
}

bool Socket::setReUseAddress() {
#ifdef _WIN32
	printf("@todo Implement setReUseAddress() on windows.\n");
#elif __APPLE__
	int set = 1;
	int result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
	return result == 0;
#endif
}

bool Socket::setKeepAlive() {
	// http://stackoverflow.com/questions/3173583/how-to-set-keepalive-on-windows-server-2008
	printf("@todo, we need to implement setKeepAlive\n");
	return false;
}

void Socket::close() {
#ifdef _WIN32
	closesocket(sock);
#elif __APPLE__
	::close(sock);
#endif
}

} // roxlu
