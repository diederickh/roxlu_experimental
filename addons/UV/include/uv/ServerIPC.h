/*
  # ServerIPC

  Simple wrapper around the unix-pipe feature of libuv which allows you to do
  super fast inter process communication. You start a "server" by creating a ServerIPC
  instance and passing it a socket file. This socket file is used as stream-pipe to transfer
  data from the server<>client. 

  Make sure to call `ServerIPC::update()` often.

 */
#ifndef ROXLU_SERVER_IPC_H
#define ROXLU_SERVER_IPC_H

extern "C" {
#  include <uv.h>
};

#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#define SERVER_IPC_ERR(r) { if (r != 0) { RX_ERROR("%s", uv_strerror(uv_last_error(loop))); } }

void server_ipc_connection_on_write(uv_write_t* req, int status);
void server_ipc_on_new_connection(uv_stream_t* server, int status);
void server_ipc_on_close(uv_handle_t* handle);
void server_ipc_on_shutdown(uv_shutdown_t* req, int status);
void server_ipc_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf);
uv_buf_t server_ipc_on_alloc(uv_handle_t* handle, size_t nbytes);

class ServerIPC;

class IPCConnection {
 public:
  IPCConnection(ServerIPC* server);
  ~IPCConnection();
  void write(char* data, size_t nbytes);
 public:
  ServerIPC* server;
  uv_pipe_t pipe;
  uv_shutdown_t shutdown_req;
  std::vector<char> buffer;
};

class ServerIPC {
 public:
  ServerIPC();
  ~ServerIPC();
  bool setup(std::string sockfile, bool datapath);
  void update();
  void removeConnection(IPCConnection* con);
  void writeToAllConnections(const char* buf, size_t nbytes);
  void writeToAllConnections(char* buf, size_t nbytes);
 public:
  uv_loop_t* loop;
  uv_pipe_t server;
  std::vector<IPCConnection*> connections;
};

inline void ServerIPC::writeToAllConnections(const char* buf, size_t nbytes) {
  writeToAllConnections((char*)buf, nbytes);
}

#endif



