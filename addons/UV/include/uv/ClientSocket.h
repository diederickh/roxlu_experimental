#ifndef ROXLU_CLIENT_SOCKET_H
#define ROXLU_CLIENT_SOCKET_H

extern "C" {
#  include <stdio.h>
#  include <uv.h>
}

#include <iterator> /* std::back_inserter */
#include <vector>
#include <string>
#include <roxlu/core/Log.h>

#define CS_WARN_CANT_DISCONNECT "We're not connected so we cant disconnect"
#define CS_ERR_CANT_SHUTDOWN "Calling uv_shutdown failed"

class ClientSocket;

typedef void(*client_socket_on_connected_cb)(ClientSocket* sock);
typedef void(*client_socket_on_read_cb)(char* buf, size_t nbytes, ClientSocket* sock);

void client_socket_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
void client_socket_on_connect(uv_connect_t* req, int status);
void client_socket_on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf);
uv_buf_t client_socket_on_alloc(uv_handle_t* handle, size_t nbytes);
void client_socket_on_write(uv_write_t* req, int status);
void client_socket_on_shutdown(uv_shutdown_t* req, int status);
void client_socket_on_shutdown_reconnect(uv_shutdown_t* req, int status);
void client_socket_on_close(uv_handle_t* req);
void client_socket_on_close_reconnect(uv_handle_t* req);
void client_socket_on_close_delete(uv_handle_t* req);
void client_socket_on_reconnect_timer(uv_timer_t* handle, int status);

class ClientSocket {
 public:
  ClientSocket(std::string host, std::string port);
  ~ClientSocket();

  void setup(client_socket_on_connected_cb conCB, 
             client_socket_on_read_cb readCB,
             void* user);

  void update();                                                /* you must call this repeatetly! each time you call update() we process a bit of socket data */
  bool connect();                                               /* connect to the server */
  void disconnect();                                            /* disconnects the current connection; if connected */
  void reconnect();                                             /* used internally; when disconnected we try to reconnect on a given time interval */
  void write(char* data, size_t nbytes);                        /* write data over sockets */
  void clear();                                                 /* clears the buffer */
  void close();                                                 /* shuts down the connection */
 public:
  uv_loop_t* loop;
  uv_tcp_t* sock;
  uv_getaddrinfo_t resolver_req;
  uv_connect_t connect_req;
  uv_shutdown_t shutdown_req;
  uv_timer_t timer_req;
  std::string host;
  std::string port;
  std::vector<char> buffer;

  client_socket_on_connected_cb cb_connected;
  client_socket_on_read_cb cb_read;
  void* user;
  
  bool is_connected;

};

#endif
