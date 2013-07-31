/*
  # ServerIPC

  Simple wrapper around the unix-pipe feature of libuv which allows you to do
  super fast inter process communication. You start a "server" by creating a ServerIPC
  instance and passing it a socket file. This socket file is used as stream-pipe to transfer
  data from the server<>client. 

  Make sure to call `ServerIPC::update()` often.

  - @todo - check if we really need to write to all separate clients instead of just to the named pipe

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
#include <uv/ipc/TypesIPC.h>
#include <uv/ipc/ParserIPC.h>

#define SERVER_IPC_ERR(r) { if (r <  0) { RX_ERROR("%s", uv_strerror(r)); } }

class ConnectionIPC;
typedef void(*server_ipc_on_read_callback)(ConnectionIPC* con, void* user);                               /* user callback; gets called when we receive data from the client; data will be stored in the `buffer` member */

void server_ipc_on_connection_write(uv_write_t* req, int status);                                         /* gets called after we've written to a client */
void server_ipc_on_connection_new(uv_stream_t* server, int status);                                       /* gets called when a new client tries to connect to the domain socket/pipe */
void server_ipc_on_connection_close(uv_handle_t* handle);                                                 /* gets called when connection with the client is lost or closed */
void server_ipc_on_connection_shutdown(uv_shutdown_t* req, int status);                                   /* gets called when we're shutting down a client connection and all writes have finished */
void server_ipc_on_connection_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf);                    /* gets called when we receive data from the client */
void server_ipc_on_server_close(uv_handle_t* handle);                                                     /* gets called when the server is closed */
uv_buf_t server_ipc_on_alloc(uv_handle_t* handle, size_t nbytes);                                         /* will allocate a uv_buf_t when necessary; called by libuv */

void connection_ipc_on_command(std::string path, char* data, size_t nbytes, void* user);                  /* gets called when the ParserIPC finds a valid command */

class ServerIPC;

class ConnectionIPC {                                                                                     /* represents a client connection */
 public:
  ConnectionIPC(ServerIPC* server);
  ~ConnectionIPC();
  void write(char* data, size_t nbytes);
  bool close();
  void parse();                                                                                          /* parses the bufer and calls the appropriate method handlers */
 public:
  ServerIPC* server;
  uv_pipe_t pipe;
  uv_shutdown_t shutdown_req;
  std::vector<char> buffer;
  ParserIPC parser;
};

class ServerIPC {
 public:
  ServerIPC(std::string sockfile, bool datapath);
  ~ServerIPC();
  bool setup(server_ipc_on_read_callback readCB, void* user);
  bool start();
  bool stop();
  void update();
  void removeConnection(ConnectionIPC* con);
  void removeAllConnections();
  void writeToAllConnections(const char* buf, size_t nbytes);
  void writeToAllConnections(char* buf, size_t nbytes);
  void addMethod(std::string path, ipc_callback cb, void* user);
  void call(std::string path, const char* buf, uint32_t nbytes);
  void callMethodHandlers(std::string path, char* buf, size_t nbytes);
 public:
  std::string sockpath;
  uv_loop_t* loop;
  uv_pipe_t server;
  uv_shutdown_t shutdown_req;
  std::vector<ConnectionIPC*> connections;
  server_ipc_on_read_callback cb_read;
  void* cb_user;
  std::vector<MethodIPC*> methods;
};


inline void ServerIPC::writeToAllConnections(const char* buf, size_t nbytes) {
  writeToAllConnections((char*)buf, nbytes);
}

#endif



