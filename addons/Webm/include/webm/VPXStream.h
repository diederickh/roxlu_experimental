#ifndef ROXLU_VPX_STREAM_H
#define ROXLU_VPX_STREAM_H

extern "C" {
#  include <uv.h>
}

#include <stdio.h>
#include <roxlu/core/Log.h>
#include <vector>

#define VPX_UV_ERR(r, okval, msg, ret)      \
  if(r != okval) { \
     RX_ERROR((msg));  \
     RX_ERROR(("uv error: %s", uv_strerror(uv_last_error(uv_default_loop())))) \
     return ret; \
  }

void vpx_server_on_new_connection(uv_stream_t* sock, int status);
void vpx_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf);
void vpx_server_on_shutdown(uv_shutdown_t* req, int status);
void vpx_server_on_close(uv_handle_t* handle);
uv_buf_t vpx_server_on_alloc(uv_handle_t* handle, size_t nbytes);

struct VPXConnection {
  VPXConnection();
  ~VPXConnection();

  uv_tcp_t sock;
  uv_shutdown_t shutdown_req;
};

struct VPXInStream {
  VPXInStream();
  ~VPXInStream();
};

struct VPXOutStream {
  VPXOutStream(int port);
  ~VPXOutStream();
  bool start();
  void update();

  std::vector<VPXConnection*> connections;

  uv_tcp_t sock;
  uv_loop_t* loop;
  int port;
  
};
#endif
