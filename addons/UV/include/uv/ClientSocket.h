#ifndef ROXLU_CLIENT_SOCKET_H
#define ROXLU_CLIENT_SOCKET_H

extern "C" {
#  include <uv.h>
}

#include <iterator> /* std::back_inserter */
#include <vector>
#include <string>
#include <roxlu/core/Log.h>

class ClientSocket;

typedef void(*client_socket_on_connected_cb)(ClientSocket* sock);
typedef void(*client_socket_on_read_cb)(char* buf, size_t nbytes, ClientSocket* sock);

void client_socket_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
void client_socket_on_connect(uv_connect_t* req, int status);
void client_socket_on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf);
uv_buf_t client_socket_on_alloc(uv_handle_t* handle, size_t nbytes);
void client_socket_on_write(uv_write_t* req, int status);
void client_socket_on_shutdown(uv_shutdown_t* req, int status);
void client_socket_on_close(uv_handle_t* req);
void client_socket_on_reconnect_timer(uv_timer_t* handle, int status);

class ClientSocket {
 public:
  ClientSocket(std::string host, std::string port);
  ~ClientSocket();
  void setup(client_socket_on_connected_cb conCB, 
             client_socket_on_read_cb readCB,
             void* user);
  void update();
  bool connect();
  void reconnect();
  void write(char* data, size_t nbytes);
  void clear();

 public:
  uv_loop_t* loop;
  uv_tcp_t sock;
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
};

#endif
