#include <uv/ServerSocket.h>
#include <roxlu/core/Log.h>
#include <algorithm>

// SERVER CALLBACKS
// ---------------------------------------------------
void server_socket_on_new_connection(uv_stream_t* sock, int status) {
  if(status < 0) {
    RX_ERROR(S_ERR_NEW_CONNECTION);
    return;
  }

  ServerSocket* server = static_cast<ServerSocket*>(sock->data);
  ServerConnection* con = new ServerConnection(server);
 
  int r = uv_tcp_init(server->loop, &con->sock);
  if(r < 0) {
    RX_ERROR(S_ERR_INIT_CLIENT);
    delete con;
    con = NULL;
    return;
  }

  r = uv_accept(sock, (uv_stream_t*)&con->sock);
  if(r < 0) {
    RX_ERROR(S_ERR_ACCEPT_CLIENT);
    delete con;
    con = NULL;
    return;
  }

  r = uv_read_start((uv_stream_t*)&con->sock, server_socket_on_alloc, server_socket_on_read);
  if(r < 0) {
    RX_ERROR(S_ERR_READ_START);
    delete con;
    con = NULL;
    return;
  }

  con->sock.data = con;
  server->connections.push_back(con);

  RX_VERBOSE(S_VERB_NEW_CONNECTION, con);  

  if(server->cb_new_connection) {
    server->cb_new_connection(con, server->cb_user);
  }

}

void server_socket_on_read(uv_stream_t* sock, ssize_t nbytes, uv_buf_t buf) {
  RX_VERBOSE(S_VERB_ON_READ, nbytes);
  ServerConnection* con = static_cast<ServerConnection*>(sock->data);

  if(nbytes < 0) {

    int r = uv_read_stop(sock);
    if(r < 0) {
      RX_ERROR(S_ERR_READ_STOP);
    }
    
    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }
    
    if(nbytes != UV_EOF) {

      if(con->server->cb_close) {
        con->server->cb_close(con, con->server->cb_user);
      }

      con->server->removeConnection(con);
      delete con;
      con = NULL;
      return;
    }

    r = uv_shutdown(&con->shutdown_req, sock, server_socket_on_shutdown);
    if(r <  0) {
      RX_ERROR(S_ERR_SHUTDOWN);

      if(con->server->cb_close) {
        con->server->cb_close(con, con->server->cb_user);
      }

      delete con;
      con = NULL;
    }

    return;
  }

  std::copy(buf.base, buf.base+nbytes, std::back_inserter(con->buffer));

  if(con->server->cb_read) {
    con->server->cb_read(con, con->server->cb_user);
  }

  if(buf.base) {
    delete[] buf.base;
    buf.base = NULL;
  }
}

void server_socket_on_shutdown(uv_shutdown_t* req, int status) {
  ServerConnection* con = static_cast<ServerConnection*>(req->data);
  uv_close((uv_handle_t*)&con->sock, server_socket_on_close);
}

void server_socket_on_close(uv_handle_t* handle) {
  ServerConnection* con = static_cast<ServerConnection*>(handle->data);

  if(con->server->cb_close) {
    con->server->cb_close(con, con->server->cb_user);
  }

  con->server->removeConnection(con);

  delete con;
  con = NULL;
}

uv_buf_t server_socket_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];

  if(!buf) {
    RX_ERROR(S_ERR_CANNOT_ALLOC_BUF);
  }

  return uv_buf_init(buf, nbytes);
}


// SERVER CONNECTION
// ---------------------------------------------------
void server_connection_on_write(uv_write_t* req, int status) {

  ServerConnection* con = static_cast<ServerConnection*>(req->data);

  if(status) {
    RX_ERROR(S_ERR_WRITE_FAILED, status);
  }

  delete req;
}

ServerConnection::ServerConnection(ServerSocket* server) 
  :server(server)
{
  sock.data = this;
  shutdown_req.data = this;
}

ServerConnection::~ServerConnection() {
}

void ServerConnection::write(const char* data, size_t nbytes) {
  uv_buf_t buf = uv_buf_init((char*)data, nbytes);
  uv_write_t* req = new uv_write_t();
  req->data = this;
  
  int r = uv_write(req, (uv_stream_t*)&sock, &buf, 1, server_connection_on_write);
  if(r < 0) {
    RX_ERROR(S_ERR_CANNOT_WRITE);
  }
}


// SERVER SOCKET
// ---------------------------------------------------
ServerSocket::ServerSocket(int port)
  :port(port)
  ,cb_new_connection(NULL)
  ,cb_read(NULL)
  ,cb_close(NULL)
  ,cb_user(NULL)
{
  loop = uv_loop_new();
}

ServerSocket::~ServerSocket() {
  RX_ERROR("Need to remove all connections.");
  cb_user = NULL;
  cb_close = NULL;
  cb_read = NULL;
  cb_new_connection = NULL;
}

void ServerSocket::setup(server_socket_new_connection_callback conCB,     
                         server_socket_read_callback readCB,
                         server_socket_close_connection_callback closeCB,
                         void* user)
{
  cb_new_connection = conCB;
  cb_read = readCB;
  cb_close = closeCB;
  cb_user = user;
}


bool ServerSocket::start() {
  if(!loop) {
    RX_ERROR(S_ERR_NO_LOOP);
    return false;
  }

  int r = uv_tcp_init(loop, &sock);
  sock.data = this;
  SERVER_UV_ERR(r, 0, S_ERR_INIT_SOCK, false);

  struct sockaddr_in baddr = uv_ip4_addr("0.0.0.0", port);
  r = uv_tcp_bind(&sock, baddr);
  SERVER_UV_ERR(r, 0, S_ERR_BIND_SOCK, false);


  r = uv_listen((uv_stream_t*)&sock, 128, server_socket_on_new_connection);
  SERVER_UV_ERR(r, 0, S_ERR_LISTEN, false);

  return true;
}


void ServerSocket::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

void ServerSocket::removeConnection(ServerConnection* con) {
  std::vector<ServerConnection*>::iterator it = std::find(connections.begin(), connections.end(), con);
  if(it == connections.end()) {
    RX_ERROR(S_ERR_CON_NOT_FOUND);
    return;
  }
  
  RX_VERBOSE(S_VERB_REMOVED_CONNECTION, con);
  connections.erase(it);
}

void ServerSocket::writeToAllConnections(const char* data, size_t nbytes) {
  for(std::vector<ServerConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    (*it)->write(data, nbytes);
  }
}
