#include <uv/ServerIPC.h>

// -----------------------------------------------------------------------------
void server_ipc_connection_on_write(uv_write_t* req, int status) {

  IPCConnection* ipc = static_cast<IPCConnection*>(req->data);

  if(status == -1) {
    RX_ERROR("Error with writing to client: %s", uv_strerror(uv_last_error(ipc->pipe.loop)));
  }

  delete req;
  req = NULL;
}

// -----------------------------------------------------------------------------
IPCConnection::IPCConnection(ServerIPC* server)
  :server(server)
{
  shutdown_req.data = this;
}

IPCConnection::~IPCConnection() {
  RX_ERROR("NEED TO CLEAN UP");
}

void IPCConnection::write(char* data, size_t nbytes) {
  uv_buf_t buf = uv_buf_init((char*)data, nbytes);
  uv_write_t* req = new uv_write_t();
  req->data = this;
  
  int r = uv_write(req, (uv_stream_t*)&pipe, &buf, 1, server_ipc_connection_on_write);
  if(r) {
    RX_ERROR("Cannot write");
  }
}

// -----------------------------------------------------------------------------
void server_ipc_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {
  RX_VERBOSE("ON READ");
  IPCConnection* ipc = static_cast<IPCConnection*>(handle->data);
  if(nbytes < 0) {
    RX_VERBOSE("DISCONNECTED CLIENT!");

    int r = uv_read_stop(handle);
    if(r) {
      RX_ERROR("Error while trying to stop reading from client");
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    uv_err_t err = uv_last_error(handle->loop);
    if(err.code != UV_EOF) {
      RX_ERROR("Unhandled error....");
      return;
    }

    r = uv_shutdown(&ipc->shutdown_req, handle, server_ipc_on_shutdown);
    if(r) {
      RX_ERROR("Error while trying to shutdown the pipe");
    }

    return;
  }

  if(buf.base) {
    std::copy(buf.base, buf.base + nbytes, std::back_inserter(ipc->buffer));
    delete[] buf.base;
    buf.base = NULL;
  }

}

uv_buf_t server_ipc_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];

  if(!buf) {
    RX_ERROR("Cannot allocate a new buffer");
  }

  return uv_buf_init(buf, nbytes);
}

void server_ipc_on_close(uv_handle_t* handle) {
  RX_VERBOSE("on close");

  IPCConnection* ipc = static_cast<IPCConnection*>(handle->data);
  ipc->server->removeConnection(ipc);

  delete ipc;
  ipc = NULL;
}

void server_ipc_on_shutdown(uv_shutdown_t* req, int status) {
  IPCConnection* con = static_cast<IPCConnection*>(req->data);
  uv_close((uv_handle_t*)&con->pipe, server_ipc_on_close);
}

void server_ipc_on_new_connection(uv_stream_t* sock, int status) {

  if(status == -1) {
    RX_ERROR("Error with new connection");
    return;
  }

  ServerIPC* server = static_cast<ServerIPC*>(sock->data);
  IPCConnection* con = new IPCConnection(server);

  int r = uv_pipe_init(server->loop, &con->pipe, 0);
  if(r) {
    RX_ERROR("Cannot initialize the connection");
    delete con;
    con = NULL;
    return;
  }

  r = uv_accept(sock, (uv_stream_t*)&con->pipe);
  if(r) {
    RX_ERROR("Cannot accept client connection");
    delete con;
    con = NULL;
    return;
  }

  r = uv_read_start((uv_stream_t*)&con->pipe, server_ipc_on_alloc, server_ipc_on_read);
  if(r) {
    RX_ERROR("Cannot start reading.");
    delete con;
    con = NULL;
    return;
  }
  
  con->pipe.data = con;
  server->connections.push_back(con);
  
  RX_VERBOSE("NEW CONNECTION");
}

// -----------------------------------------------------------------------------

ServerIPC::ServerIPC() 
  :loop(NULL)
{
  loop = uv_loop_new();
  if(!loop) {
    RX_ERROR("Cannot create a new uv_loop");
    ::exit(EXIT_FAILURE);
  }

  server.data = this;
}

ServerIPC::~ServerIPC() {
  uv_close((uv_handle_t*)&server, server_ipc_on_close);
  if(loop) {
    uv_loop_delete(loop);
    loop = NULL;
  }
}


bool ServerIPC::setup(std::string sockfile, bool datapath) {
  int r = 0;

  if(!loop) {
    return false;
  }
  
  r = uv_pipe_init(loop, &server, 0);
  if(r != 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  if(datapath) {
    sockfile = rx_to_data_path(sockfile);
  }

  r = uv_pipe_bind(&server, sockfile.c_str());
  if(r != 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  r = uv_listen((uv_stream_t*)&server, 128, server_ipc_on_new_connection);
  if(r != 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  return true;
}


void ServerIPC::update() {
  uv_run(loop, UV_RUN_NOWAIT); 
}

void ServerIPC::removeConnection(IPCConnection* con) {
  std::vector<IPCConnection*>::iterator it = std::find(connections.begin(), connections.end(), con); // connections.find(con);
  if(it == connections.end()) {
    RX_ERROR("Cannot find the given connection: %p", con);
    return;
  }

  connections.erase(it);
}

void ServerIPC::writeToAllConnections(char* buf, size_t nbytes) {
  for(std::vector<IPCConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    (*it)->write(buf, nbytes);
  }
}
