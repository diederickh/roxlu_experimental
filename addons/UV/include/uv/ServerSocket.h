#ifndef ROXLU_SERVER_SOCKET_H
#define ROXLU_SERVER_SOCKET_H

extern "C" {
#  include <stdio.h>
#  include <uv.h>
}

#include <vector>
#include <iterator>

#define S_VERB_NEW_CONNECTION "Got a new connection: %p"
#define S_VERB_REMOVED_CONNECTION "Removed connection: %p"
#define S_VERB_ON_READ "Got data from client. nbytes: %ld"
#define S_ERR_NO_LOOP "Server loop member is not initialized"
#define S_ERR_INIT_SOCK "Cannot setup the server socket"
#define S_ERR_BIND_SOCK "Canont bind socket on the given port"
#define S_ERR_CANNOT_ALLOC_BUF "Cannot allocate new buffer to read incoming data"
#define S_ERR_LISTEN "Cannot start listening on port"
#define S_ERR_NEW_CONNECTION "Error while accepting new connection"
#define S_ERR_INIT_CLIENT "Cannot initialize client socket"
#define S_ERR_ACCEPT_CLIENT "Cannot accept client socket"
#define S_ERR_READ_START "Failed to start reading from client socket"
#define S_ERR_READ_STOP "Failed to disable reading on clent socket"
#define S_ERR_SHUTDOWN "Failed to call uv_shutdown for a client socket"
#define S_ERR_CON_NOT_FOUND "Cannot remove the connection because we didn't find it"
#define S_ERR_CANNOT_WRITE "Cannot write to connection"
#define S_ERR_WRITE_FAILED "Writing to connection failed: %s"

#define SERVER_UV_ERR(r, okval, msg, ret)      \
  if(r != okval) { \
     RX_ERROR(msg);  \
     RX_ERROR("uv error: %s", uv_strerror(r));  \
     return ret; \
  }

class ServerSocket;
class ServerConnection;

/* server connection specific callbacks */
void server_connection_on_write(uv_write_t* req, int status);

/* server libuv callbacks */
void server_socket_on_new_connection(uv_stream_t* sock, int status);
void server_socket_on_read(uv_stream_t* sock, ssize_t nbytes, uv_buf_t buf);
void server_socket_on_shutdown(uv_shutdown_t* req, int status);
void server_socket_on_close(uv_handle_t* handle);
uv_buf_t server_socket_on_alloc(uv_handle_t* handle, size_t nbytes);

/* custom callbacks */
typedef void(*server_socket_new_connection_callback)(ServerConnection* con, void* user); /* gets called when a new client connects */
typedef void(*server_socket_read_callback)(ServerConnection* con, void* user); /* gets called when we received new data which was appended to ServerConnection::buffer */
typedef void(*server_socket_close_connection_callback)(ServerConnection* con, void* user); /* gets called when a client disconnects */

struct ServerConnection {                                     /* represents a client connection */
  ServerConnection(ServerSocket* server);
  ~ServerConnection();
  void write(const char* data, size_t bytes);                 /* write data to the client */
  uv_tcp_t sock;                                              /* the socket on which we communicate */
  uv_shutdown_t shutdown_req;                                 /* used internally to correctly close a client connection */
  ServerSocket* server;                                       /* points to the server */
  std::vector<char> buffer;
};

class ServerSocket {                                         /* the server socket, which manages tcp connections */
  public:
  ServerSocket(int port);                                      /* create a server which listens on the given port; for simplicity we start listening on all eth-devices */
  ~ServerSocket();                                             /* d'tor; cleans up all memory; closes clients */

  void setup(server_socket_new_connection_callback conCB,      /* setup the server with the given callbacks */
             server_socket_read_callback readCB,
             server_socket_close_connection_callback closeCB,
             void* user
  );

  bool start();                                                 /* start the server and accepting new connections */
  void update();                                                /* call this as often as possible */
  void removeConnection(ServerConnection* con);                 /* is used internally to cleanup when a client disconnects */
  void writeToAllConnections(const char* data, size_t nbytes);        
 public:
  uv_loop_t* loop;
  uv_tcp_t sock;
  int port;
  std::vector<ServerConnection*> connections;

  server_socket_new_connection_callback cb_new_connection;
  server_socket_read_callback cb_read;
  server_socket_close_connection_callback cb_close;
  void* cb_user;
  
};

#endif
