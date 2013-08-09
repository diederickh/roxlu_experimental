#include <uv/ipc/ClientIPC.h>

void client_ipc_on_connect(uv_connect_t* req, int status) {
  ClientIPC* ipc = static_cast<ClientIPC*>(req->data);

  if(status < 0) {
    RX_ERROR("Something went wrong when trying to connect to ipc server: %s", uv_strerror(status));
    //ipc->state = CIPS_ST_RECONNECTING;;
    ipc->reconnect();
    return;
  }

  int r = uv_read_start((uv_stream_t*)&ipc->pipe, client_ipc_on_alloc, client_ipc_on_read);
  if(r < 0) {
    RX_ERROR("Cannot start reading: %s", uv_strerror(r));
    //ipc->state = CIPS_ST_RECONNECTING;
    ipc->reconnect();
    return;
  }

  ipc->buffer.clear(); 
  ipc->state = CIPS_ST_CONNECTED;

  RX_VERBOSE("Connected!");
}

void client_ipc_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {

  ClientIPC* ipc = static_cast<ClientIPC*>(handle->data);
  if(nbytes < 0) {

    int r = uv_read_stop(handle);
    if(r < 0) {
      RX_ERROR("Cannot stop reading: %s", uv_strerror(r));
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    if(nbytes != UV_EOF) {
      RX_ERROR("Unhandled error");
    }

    r = uv_shutdown(&ipc->shutdown_req, handle, client_ipc_on_shutdown);
    if(r < 0) {
      RX_ERROR("Error shutting down client: %s // @TODO do we need to delete ClientIPC here?", uv_strerror(r));
    }

    return;
  }
    
  if(buf.base) {
    RX_VERBOSE("PARSE CLIENT BUFFER");
    std::copy(buf.base, buf.base + nbytes, std::back_inserter(ipc->buffer));
    ipc->parse();

    if(ipc->cb_read) {
      ipc->cb_read(ipc, ipc->cb_user);
    }

    delete[] buf.base;
    buf.base = NULL;
  }
}

uv_buf_t client_ipc_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  return uv_buf_init(buf, nbytes);
}

void client_ipc_on_shutdown(uv_shutdown_t* req, int status) {
  ClientIPC* ipc = static_cast<ClientIPC*>(req->data);
  uv_close((uv_handle_t*)&ipc->pipe, client_ipc_on_close);
}

void client_ipc_on_close(uv_handle_t* handle) {
  ClientIPC* ipc = static_cast<ClientIPC*>(handle->data);
  ipc->state = CIPS_ST_RECONNECTING;
  ipc->reconnect_timeout = (uv_hrtime() / 1000000) + ipc->reconnect_delay;
}

void client_ipc_on_write(uv_write_t* req, int status) {

  if(status < 0) {
    ClientIPC* ipc = static_cast<ClientIPC*>(req->data);
    ipc->reconnect();
    RX_ERROR("%s", uv_strerror(status));
  }

  delete req;
  req = NULL;
}

// -----------------------------------------------

void client_ipc_on_command(std::string path, char* data, size_t nbytes, void* user) {
  ClientIPC* ipc = static_cast<ClientIPC*>(user);
  ipc->callMethodHandlers(path, data, nbytes);
}

// -----------------------------------------------

ClientIPC::ClientIPC(std::string sockfile, bool datapath)
  :loop(NULL)
  ,cb_con(NULL)
  ,cb_read(NULL)
  ,cb_user(NULL)
  ,reconnect_delay(5000)
  ,state(CIPS_ST_NONE)
  ,reconnect_timeout(0)
{

  loop = uv_loop_new();
  if(!loop) {
    RX_ERROR("Cannot create a new loop");
    ::exit(EXIT_FAILURE);
  }

  sockpath = sockfile;
  if(datapath) {
    sockpath = rx_to_data_path(sockfile);
  }
  
  connect_req.data = this;
  shutdown_req.data = this;

  parser.cb_parser = client_ipc_on_command;
  parser.cb_user = this;
}

ClientIPC::~ClientIPC() {
  cb_user = NULL;
  cb_read = NULL;
  cb_con = NULL;
}

bool ClientIPC::setup(client_ipc_on_connected_cb conCB,  
                      client_ipc_on_read_cb readCB, 
                      void* user)
{
  cb_con = conCB;
  cb_read = readCB;
  cb_user = user;
  return true;
}

bool ClientIPC::connect() {

#if 0
  // We don't need to check if the file exist, uv_pipe_connect() will give an error
  // @todo we should remove this after a while 
  if(!rx_file_exists(sockpath)) {
    RX_ERROR("Cannot connect through ipc; the pipe is not found: `%s`", sockpath.c_str());
    return false;
  }
#endif

  if(sockpath.size() >= 127) {
    RX_ERROR("Unix socket paths should have a length < 127");
    return false;
  }

  int r = uv_pipe_init(loop, &pipe, 0);
  if(r < 0) {
    RX_ERROR("Error setting up pipe: %s", uv_strerror(r));
    return false;
  }
  
  pipe.data = this;
  state = CIPS_ST_CONNECTING;
  
  uv_pipe_connect(&connect_req, &pipe, sockpath.c_str(), client_ipc_on_connect);
  return true;
}

void ClientIPC::update() {
  if(state == CIPS_ST_RECONNECTING) {
    uint64_t now = uv_hrtime() / 1000000;
    //RX_VERBOSE("NOW: %lld, TIMEOUT: %lld", reconnect_timeout);
    if(reconnect_timeout <= now) {
      RX_VERBOSE("Reconnecting");
      reconnect_timeout = now + reconnect_delay;
      connect();
    }
  }
  uv_run(loop, UV_RUN_NOWAIT);
}

bool ClientIPC::reconnect() {

  if(state == CIPS_ST_RECONNECTING) {
    RX_VERBOSE("Cannot reconnect; already trying");
    return false;
  }

  if(state != CIPS_ST_RECONNECTING) {
    state = CIPS_ST_RECONNECTING;
    reconnect_timeout = (uv_hrtime()/1000000) + reconnect_delay;
  }
  RX_WARNING(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RECONNECTING <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  return true;
}

void ClientIPC::write(char* data, size_t nbytes) {

  if(!uv_is_writable((uv_stream_t*)&pipe)) {
    RX_ERROR("Cannot write to server IPC; probably you didn't connect to a service yet, current state: %d", state);
    return;
  }

  // @todo -> add a check if we're connected; else this results in a segfault
#if 0  
  ClientWrite* cw = new ClientWrite;
  cw->buf = uv_buf_init(data, nbytes);
  cw->req.data = this;
  int r = uv_write(&cw->req, (uv_stream_t*)&pipe, &cw->buf, 1, client_ipc_on_write);
#else 
  // @todo - check if we're connected
  uv_write_t* req = new uv_write_t();
  req->data = this;
  uv_buf_t buf = uv_buf_init(data, nbytes);
  int r = uv_write(req, (uv_stream_t*)&pipe, &buf, 1, client_ipc_on_write);

#endif
  
  if(r < 0) {
    RX_ERROR("Error writing: %s", uv_strerror(r));
    if(state != CIPS_ST_CONNECTED && state != CIPS_ST_CONNECTING) {
      reconnect();
    }
  }
}

void ClientIPC::call(std::string path, const char* data, uint32_t nbytes) {
  uint32_t cmd = METHOD_IPC_COMMAND;
  uint32_t path_len = path.size();

  write((char*)&cmd, sizeof(cmd));

  write((char*)&path_len, sizeof(path_len));
  write((char*)path.c_str(), path_len);
  
  write((char*)&nbytes, sizeof(nbytes));

  RX_VERBOSE("+++++++++++++++++++++++ Writing command: %s with %ld bytes of data.", path.c_str(), nbytes);

  if(nbytes > 0) {
    write((char*)data, nbytes);
  }
}

void ClientIPC::addMethod(std::string path, ipc_callback cb, void* user) {
  MethodIPC* me = new MethodIPC();
  me->cb_user = user;
  me->cb_path = cb;
  me->path = path;
  me->id = rx_string_id(path);
  methods.push_back(me);
}

void ClientIPC::callMethodHandlers(std::string path, char* data, size_t nbytes) {
  uint32_t id = rx_string_id(path);

  for(std::vector<MethodIPC*>::iterator it = methods.begin(); it != methods.end(); ++it) {

    MethodIPC* me = *it;
    if(me->id == id && me->cb_path) {
      me->cb_path(path, data, nbytes, me->cb_user);
    }
  }
}

void ClientIPC::parse() {

  parser.parse(buffer);

#if 0
  uint32_t cmd;
  size_t cmd_offset = sizeof(cmd) + sizeof(uint32_t);
  size_t cmd_nbytes = 0;
  
  size_t data_offset = 0;
  size_t data_nbytes = 0;;

  while(buffer.size() > cmd_offset) {
    memcpy((char*)&cmd, &buffer[0], sizeof(cmd));
    memcpy((char*)&cmd_nbytes, &buffer[sizeof(cmd)], sizeof(uint32_t)); 

    if(cmd != METHOD_IPC_COMMAND) {
      return;
    }

    if(buffer.size() < cmd_nbytes + cmd_offset) {
      return;
    }
        
    // do we have a complete conmand?
    std::string path(&buffer[cmd_offset], cmd_nbytes);
    data_offset = sizeof(cmd) + sizeof(uint32_t) + cmd_nbytes;
    if(buffer.size() <= data_offset) {
      return;
    }

    memcpy((char*)&data_nbytes, &buffer[data_offset], sizeof(uint32_t));
    if(buffer.size() <= (data_offset + data_nbytes)) {
      return;
    }

    callMethodHandlers(path, data_nbytes ? &buffer[data_offset + sizeof(uint32_t)] : NULL, data_nbytes);
      
    // remove the data
    buffer.erase(buffer.begin(), buffer.begin() + data_offset + sizeof(uint32_t) + data_nbytes);
  }
#endif
}
