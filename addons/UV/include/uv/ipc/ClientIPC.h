/* 

   # ClientIPC

   This class is used as the client side for pipe based inter process communication (IPC). 
   The `ServerIPC` and `ClientIPC` classes allow you to create a safe and super fast communication
   system between multiple processes.

   Make sure to call `update()` often.

*/
#ifndef ROXLU_CLIENT_IPC_H
#define ROXLU_CLIENT_IPC_H

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

#define CIPS_ST_NONE 0                                 /* default state */
#define CIPS_ST_CONNECTING 1                           /* when we're currently connecting */
#define CIPS_ST_CONNECTED 2                            /* connected */
#define CIPS_ST_RECONNECTING 3                         /* we will automatically reconnect when we get disconnected */

class ClientIPC;
typedef void(*client_ipc_on_connected_cb)(ClientIPC* ipc, void* user);
typedef void(*client_ipc_on_read_cb)(ClientIPC* ipc, void* user);

void client_ipc_on_connect(uv_connect_t* req, int status);
void client_ipc_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf);
void client_ipc_on_write(uv_write_t* req, int status);
void client_ipc_on_shutdown(uv_shutdown_t* req, int status);
void client_ipc_on_close(uv_handle_t* handle);
uv_buf_t client_ipc_on_alloc(uv_handle_t* handle, size_t nbytes);

void client_ipc_on_command(std::string path, char* data, size_t nbytes, void* user);  /* gets called when ParserIPC finds a valid command */

class ClientIPC {
 public:
  ClientIPC(std::string sockfile, bool datapath);
  ~ClientIPC();
  bool setup(client_ipc_on_connected_cb conCB,  
             client_ipc_on_read_cb readCB, 
             void* user);
  bool connect();
  bool isConnected();
  bool reconnect();
  void update();
  void write(char* data, size_t nbytes);
  void write(const char* data, size_t nbytes);
  void call(std::string path, const char* data, uint32_t nbytes);
  void addMethod(std::string path, ipc_callback cb, void* user);
  void callMethodHandlers(std::string path, char* buf, size_t nbytes);
  void parse();                                                           /* parses the incoming buffer and calls the necessary callbacks which have been added */
 public:
  std::string sockpath;
  uv_loop_t* loop;
  uv_pipe_t pipe;
  uv_connect_t connect_req;
  uv_shutdown_t shutdown_req;
  client_ipc_on_connected_cb cb_con;
  client_ipc_on_read_cb cb_read;
  void* cb_user;
  std::vector<char> buffer;                                            /* buffer that contains the received data; will on be used when a read callback is set; the read callback should flush the buffer when necessary */
  int state;
  uint64_t reconnect_delay;                                            /* try to reconnect after X-millis */
  uint64_t reconnect_timeout;
  std::vector<MethodIPC*> methods;                                     /* method handlers; see `addMethod()` */
  ParserIPC parser;
};

inline void ClientIPC::write(const char* data, size_t nbytes) {
  write((char*)data, nbytes);
}

inline bool ClientIPC::isConnected() {
  return !uv_is_closing((uv_handle_t*)&pipe) && uv_is_writable((uv_stream_t*)&pipe);
}

#endif
