#include <webm/VPXStream.h>

// IN STREAM
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXInStream::VPXInStream() {
  RX_VERBOSE(("VPXInStream()"));
}

VPXInStream::~VPXInStream() {
}

// OUT STREAM
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VPXOutStream::VPXOutStream(int port) 
  :port(port)
  ,loop(NULL)
{
  RX_VERBOSE(("VPXOutStream()"));
  sock.data = this;
}

VPXOutStream::~VPXOutStream() {
}

bool VPXOutStream::start() {
  loop = uv_default_loop();
  if(!loop) {
    RX_ERROR(("Cannot create loop"));
    return false;
  }

  int r = 0;
  r = uv_tcp_init(loop, &sock);
  VPX_UV_ERR(r, 0, "uv_tcp_init failed", false);

  struct sockaddr_in baddr = uv_ip4_addr("0.0.0.0", port);
  r = uv_tcp_bind(&sock, baddr);
  VPX_UV_ERR(r, 0, "uv_tcp_bind failed", false);

  r = uv_listen((uv_stream_t*)&sock, 128, vpx_server_on_new_connection);
  VPX_UV_ERR(r, 0, "uv_listen failed", false);

  return true;
}

void VPXOutStream::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

// Connection
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// UV 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void vpx_server_on_new_connection(uv_stream_t* sock, int status) {
  RX_VERBOSE(("- on new connection -"));
}

void vpx_server_on_read(uv_stream_t* sock, ssize_t nread, uv_buf_t buf) {
}

void vpx_server_on_shutdown(uv_shutdown_t* req, int status) {
}

void vpx_server_on_close(uv_handle_t* handle) {
}

uv_buf_t vpx_server_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  if(!buf) {
    RX_ERROR(("ERROR - OUT OF MEMORY IN VPX STREAM"));
  }
  return uv_buf_init(buf, nbytes);
}
