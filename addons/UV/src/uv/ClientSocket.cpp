#include <uv/ClientSocket.h>


// CLIENT SOCKET
// ------------------------------------------
ClientSocket::ClientSocket(std::string host, std::string port)
  :host(host)
  ,port(port)
  ,loop(NULL)
  ,user(NULL)
   //  ,timer_req(NULL)
  ,cb_connected(NULL)
  ,cb_read(NULL)
  ,is_connected(false)
  ,is_connecting(false)
{
  loop = uv_loop_new();

  sock = new uv_tcp_t();
  sock->data = this;

  resolver_req.data = this;
  connect_req.data = this;
  shutdown_req.data = this;
  timer_req.data = this;
}

ClientSocket::~ClientSocket() {
  if(is_connected) {
    close();
  }

  user = NULL;
  cb_connected = NULL;
  cb_read = NULL;

  uv_loop_delete(loop);
  loop = NULL;
  is_connected = false;
  is_connecting = false;
}

void ClientSocket::setup(client_socket_on_connected_cb conCB, 
                         client_socket_on_read_cb readCB,
                         void* userData) {
  cb_connected = conCB;
  cb_read = readCB;
  user = userData;
}

bool ClientSocket::connect() {

  if(is_connecting) {
    RX_WARNING(CS_WARN_RECONNECTING);
    return false;
  }

  is_connecting = true;

  int r = uv_tcp_init(loop, sock);
  if(r < 0) {
    RX_ERROR("uv_tcp_init failed");
  }

  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
 
  r = uv_getaddrinfo(loop, &resolver_req,client_socket_on_resolved, 
                     host.c_str(), port.c_str(), &hints);
 
  if(r < 0) {
    RX_ERROR("cannot uv_tcp_init(): %s", uv_strerror(r));
    return false;
  }
    
  return true;
}

void ClientSocket::disconnect() {
  if(!is_connected) {
    RX_WARNING(CS_WARN_CANT_DISCONNECT);
    return;
  }

  int r = uv_shutdown(&shutdown_req, (uv_stream_t*)sock, client_socket_on_shutdown);
  if(r < 0) {
    RX_ERROR(CS_ERR_CANT_SHUTDOWN);
  }

}

void ClientSocket::reconnect() {
  if(is_connecting) {
    return;
  }

  int r = uv_timer_init(loop, &timer_req);
  if(r < 0) {
    RX_ERROR("uv_time_init() failed. cannot reconnect");
    return;
  }

  uv_timer_start(&timer_req, client_socket_on_reconnect_timer, 10000, 0);
}

void ClientSocket::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

void ClientSocket::write(char* data, size_t nbytes) {
  if(!is_connected) {
    RX_ERROR(CS_ERR_CANT_WRITE);
    return;
  }

  uv_buf_t buf = uv_buf_init(data, nbytes);
  uv_write_t* wreq = new uv_write_t();
  wreq->data = this;

  int r = uv_write(wreq, (uv_stream_t*)sock, &buf, 1, client_socket_on_write);
  if(r < 0) {
    RX_ERROR("uv_write() to server failed.");
  }
}

void ClientSocket::close() {
  uv_close((uv_handle_t*)sock, client_socket_on_close_delete);
}

// CALLBACKS
// ------------------------------------------
void client_socket_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {
  RX_VERBOSE("resolved with status: %d", status);
  ClientSocket* c = static_cast<ClientSocket*>(req->data);
  if(status < 0) {
    RX_ERROR("cannot resolve(): %s", uv_strerror(status));
    c->reconnect();
    return;
  }

  char ip[17] = {0};
  uv_ip4_name((struct sockaddr_in*)res->ai_addr, ip, 16);
  RX_VERBOSE("resolved server: %s", ip);
 
  int r = uv_tcp_connect(&c->connect_req, c->sock, 
                         *(struct sockaddr_in*)res->ai_addr, 
                         client_socket_on_connect);
 
  uv_freeaddrinfo(res);
}

void client_socket_on_connect(uv_connect_t* req, int status) {
  ClientSocket* c = static_cast<ClientSocket*>(req->data);
  if(status < 0) {
    RX_ERROR("cannot connect: %s", uv_strerror(status));
    c->is_connecting = false;
    c->reconnect();
    return;
  }

  RX_VERBOSE("ClietSocket::sock: %p, type: %d, UV_TCP: %d", c->sock, c->sock->type, UV_TCP);

  int r = uv_read_start((uv_stream_t*)c->sock, client_socket_on_alloc, client_socket_on_read);
  if(r < 0) {
    RX_ERROR("uv_read_start() failed %s", uv_strerror(r));
    return;
  }

  c->is_connected = true;

  if(c->cb_connected) {
    c->cb_connected(c);
  }
}


void client_socket_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {

  ClientSocket* c = static_cast<ClientSocket*>(handle->data);

  if(nbytes < 0) {
    int r = uv_read_stop(handle);
    if(r) {
      RX_ERROR("error uv_read_stop on client. %s", uv_strerror(r));
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

#if 0
    uv_err_t err = uv_last_error(handle->loop);
    if(err.code != UV_EOF) {
      RX_ERROR("disconnected from server, but not correctly!");
      return;
    }
#endif

    c->is_connected = false;

    r = uv_shutdown(&c->shutdown_req, handle, client_socket_on_shutdown_reconnect);
    if(r < 0) {
      RX_ERROR("error shutting down client. %s", uv_strerror(r));
      delete c;
      c = NULL;
      return;
    }
       
    return;
  }

  if(c->cb_read) {
    c->cb_read(buf.base, nbytes, c);
  }

  if(buf.base) {
    delete[] buf.base;
    buf.base = NULL;
  }

}

uv_buf_t client_socket_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  return uv_buf_init(buf, nbytes);
}

void client_socket_on_write(uv_write_t* req, int status) {
  delete req;
}

void client_socket_on_shutdown(uv_shutdown_t* req, int status) {
  ClientSocket* c = static_cast<ClientSocket*>(req->data);
  uv_close((uv_handle_t*)c->sock, client_socket_on_close);
}

void client_socket_on_shutdown_reconnect(uv_shutdown_t* req, int status) {
  ClientSocket* c = static_cast<ClientSocket*>(req->data);
  uv_close((uv_handle_t*)c->sock, client_socket_on_close_reconnect);
}


void client_socket_on_close(uv_handle_t* handle) {
  ClientSocket* c = static_cast<ClientSocket*>(handle->data);
  c->is_connected = false;
}

void client_socket_on_close_reconnect(uv_handle_t* handle) {
  ClientSocket* c = static_cast<ClientSocket*>(handle->data);
  c->is_connected = false;
  c->is_connecting = false;
  c->reconnect();
}

void client_socket_on_close_delete(uv_handle_t* handle) {
  delete handle;
  handle = NULL;
}

void client_socket_on_reconnect_timer(uv_timer_t* handle, int status) {
  ClientSocket* c = static_cast<ClientSocket*>(handle->data);
  if(status < 0) {
    RX_ERROR("error shutting down client. %s", uv_strerror(status));
    return;
  }

  c->connect();
}
