#include <uv/ipc/ServerIPC.h>

// -----------------------------------------------------------------------------
void server_ipc_on_connection_write(uv_write_t* req, int status) {

  ConnectionIPC* ipc = static_cast<ConnectionIPC*>(req->data);

  if(status < 0) {
    RX_ERROR("Error with writing to client: %s", uv_strerror(status));
  }

  delete req;
  req = NULL;
}

void connection_ipc_on_command(std::string path, char* data, size_t nbytes, void* user) {
  ConnectionIPC* ipc = static_cast<ConnectionIPC*>(user);
  ipc->server->callMethodHandlers(path, data, nbytes); 
}

// -----------------------------------------------------------------------------
ConnectionIPC::ConnectionIPC(ServerIPC* server)
  :server(server)
{
  shutdown_req.data = this;
  parser.cb_parser = connection_ipc_on_command;
  parser.cb_user = this;
}

ConnectionIPC::~ConnectionIPC() {
  
}

void ConnectionIPC::write(char* data, size_t nbytes) {
  uv_buf_t buf = uv_buf_init((char*)data, nbytes);
  uv_write_t* req = new uv_write_t();
  req->data = this;

  int r = uv_write(req, (uv_stream_t*)&pipe, &buf, 1, server_ipc_on_connection_write);
  if(r < 0) {
    RX_ERROR("Error cannot write to client: %s", uv_strerror(r));
  }
}

bool ConnectionIPC::close() {
  int r = uv_shutdown(&shutdown_req, (uv_stream_t*)&pipe, server_ipc_on_connection_shutdown);
  if(r < 0) {
    RX_ERROR("Error closing the client: %s", uv_strerror(r));
    return false;
  }
  return true;
}



void ConnectionIPC::parse() {
  parser.parse(buffer);
}

// -----------------------------------------------------------------------------

void server_ipc_on_connection_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {

  ConnectionIPC* ipc = static_cast<ConnectionIPC*>(handle->data);
  if(nbytes < 0) {

    RX_VERBOSE("%s", uv_strerror(nbytes));

    int r = uv_read_stop(handle);
    if(r < 0) {
      RX_ERROR("Error while trying to stop reading from client");
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    r = uv_shutdown(&ipc->shutdown_req, handle, server_ipc_on_connection_shutdown);
    if(r < 0) {
      RX_ERROR("@todo - Error while trying to shutdown the pipe (when would we arrive here?)");
    }

    return;
  }

  if(buf.base) {

    std::copy(buf.base, buf.base + nbytes, std::back_inserter(ipc->buffer));
    ipc->parse();

    if(ipc->server->cb_read) {
      ipc->server->cb_read(ipc, ipc->server->cb_user);
    }

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

void server_ipc_on_connection_close(uv_handle_t* handle) {

  ConnectionIPC* ipc = static_cast<ConnectionIPC*>(handle->data);
  ipc->server->removeConnection(ipc);

  delete ipc;
  ipc = NULL;
}

void server_ipc_on_connection_shutdown(uv_shutdown_t* req, int status) {
  ConnectionIPC* con = static_cast<ConnectionIPC*>(req->data);
  uv_close((uv_handle_t*)&con->pipe, server_ipc_on_connection_close);
}

void server_ipc_on_connection_new(uv_stream_t* sock, int status) {
  if(status < 0) {
    RX_ERROR("Error with new connection");
    return;
  }

  ServerIPC* server = static_cast<ServerIPC*>(sock->data);
  ConnectionIPC* con = new ConnectionIPC(server);

  int r = uv_pipe_init(server->loop, &con->pipe, 0);
  if(r < 0) {
    RX_ERROR("Cannot initialize the connection: %s", uv_strerror(r));
    delete con;
    con = NULL;
    return;
  }

  r = uv_accept(sock, (uv_stream_t*)&con->pipe);
  if(r < 0) {
    RX_ERROR("Cannot accept client connection: %s", uv_strerror(r));
    delete con;
    con = NULL;
    return;
  }

  r = uv_read_start((uv_stream_t*)&con->pipe, server_ipc_on_alloc, server_ipc_on_connection_read);
  if(r < 0) {
    RX_ERROR("Cannot start reading: %s", uv_strerror(r));
    delete con;
    con = NULL;
    return;
  }
  
  con->pipe.data = con;
  server->connections.push_back(con);
  RX_VERBOSE("New connection!");
}

// -----------------------------------------------------------------------------

void server_ipc_on_server_close(uv_handle_t* handle) {
  ServerIPC* ipc = static_cast<ServerIPC*>(handle->data);
  ipc->removeAllConnections();
}

// -----------------------------------------------------------------------------

ServerIPC::ServerIPC(std::string sockfile, bool datapath) 
  :loop(NULL)
  ,cb_read(NULL)
  ,cb_user(NULL)
{
  loop = uv_loop_new();

  if(!loop) {
    RX_ERROR("Cannot create a new uv_loop");
    ::exit(EXIT_FAILURE);
  }

  sockpath = sockfile;
  if(datapath) {
    sockpath = rx_to_data_path(sockfile);
  }

  server.data = this;
  shutdown_req.data = this;
}

ServerIPC::~ServerIPC() {

  if(!uv_is_closing((uv_handle_t*)&server)) {
    stop();
  }
  
  // run untill the even loop is empty (we must do this so the 'stop()' is actually executed
  uv_run(loop, UV_RUN_DEFAULT);

  if(loop) {
    uv_loop_delete(loop);
    loop = NULL;
  }
  
  cb_read = NULL;
  cb_user = NULL;

  for(std::vector<MethodIPC*>::iterator it = methods.begin(); it != methods.end(); ++it) {
    MethodIPC* me = *it;
    delete me;
  }
  methods.clear();
}


bool ServerIPC::setup(server_ipc_on_read_callback readCB, void* user) {
  cb_read = readCB;
  cb_user = user;
  return true;
}

bool ServerIPC::start() {
  int r = 0; 

  if(!loop) {
    return false;
  }

  if(sockpath.size() >= 127) {
    RX_ERROR("Unix socket path should have a length < 127 characters");
    return false;
  }
  
  r = uv_pipe_init(loop, &server, 0);
  if(r < 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  if(rx_file_exists(sockpath)) {
    RX_VERBOSE("The IPC socket already existed; but we'll remove it");
    if(!rx_file_remove(sockpath)) {
      RX_ERROR("Cannot remove the already existing IPC file.. maybe permissions? `%s`", sockpath.c_str());
      return false;
    }
  }

  r = uv_pipe_bind(&server, sockpath.c_str());
  if(r < 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  r = uv_listen((uv_stream_t*)&server, 128, server_ipc_on_connection_new);
  if(r < 0) {
    SERVER_IPC_ERR(r);
    return false;
  }

  return true;
}

bool ServerIPC::stop() {

  if(uv_is_closing((uv_handle_t*)&server)) {
    RX_WARNING("Already closing");
    return false;
  }

  uv_close((uv_handle_t*)&server, server_ipc_on_server_close);
  return true;
}

void ServerIPC::update() {
  uv_run(loop, UV_RUN_NOWAIT); 
}

void ServerIPC::removeConnection(ConnectionIPC* con) {
  std::vector<ConnectionIPC*>::iterator it = std::find(connections.begin(), connections.end(), con); // connections.find(con);
  if(it == connections.end()) {
    RX_ERROR("Cannot find the given connection: %p", con);
    return;
  }

  connections.erase(it);

  RX_VERBOSE("Removing connection from server. (%ld left)", connections.size());
}

void ServerIPC::writeToAllConnections(char* buf, size_t nbytes) {
  for(std::vector<ConnectionIPC*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    (*it)->write(buf, nbytes);
  }
}

void ServerIPC::removeAllConnections() {
  for(std::vector<ConnectionIPC*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    (*it)->close();
  }
}

void ServerIPC::call(std::string path, const char* data, uint32_t nbytes) {

  uint32_t cmd = METHOD_IPC_COMMAND;
  uint32_t path_len = path.size();

  writeToAllConnections((char*)&cmd, sizeof(cmd));

  writeToAllConnections((char*)&path_len, sizeof(path_len));
  writeToAllConnections((char*)path.c_str(), path_len);
  
  writeToAllConnections((char*)&nbytes, sizeof(nbytes));

  if(nbytes > 0) {
    writeToAllConnections((char*)data, nbytes);
  }

}


void ServerIPC::addMethod(std::string path, ipc_callback cb, void* user) {
  MethodIPC* me = new MethodIPC();
  me->cb_user = user;
  me->cb_path = cb;
  me->path = path;
  me->id = rx_string_id(path);
  methods.push_back(me);
}

void ServerIPC::callMethodHandlers(std::string path, char* data, size_t nbytes) {
  uint32_t id = rx_string_id(path);

  for(std::vector<MethodIPC*>::iterator it = methods.begin(); it != methods.end(); ++it) {

    MethodIPC* me = *it;
    if(me->id == id && me->cb_path) {
      me->cb_path(path, data, nbytes, me->cb_user);
    }
  }
}
