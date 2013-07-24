#include <uv/ClientIPC.h>

void client_ipc_on_connect(uv_connect_t* req, int status) {
  RX_VERBOSE("CLIENT IPC CONNECTED");

  ClientIPC* ipc = static_cast<ClientIPC*>(req->data);

  if(status == -1) {
    RX_ERROR("Something went wrong when trying to connect to ipc server: %s", uv_strerror(uv_last_error(ipc->loop)));
    return;
  }

  int r = uv_read_start((uv_stream_t*)&ipc->pipe, client_ipc_on_alloc, client_ipc_on_read);
  if(r) {
    RX_ERROR("Cannot start reading: %s", uv_strerror(uv_last_error(ipc->loop)));
    return;
  }
}

void client_ipc_on_read(uv_stream_t* handle, ssize_t nbytes, uv_buf_t buf) {
  ClientIPC* ipc = static_cast<ClientIPC*>(handle->data);
  if(nbytes < 0) {

    int r = uv_read_stop(handle);
    if(r) {
      RX_ERROR("Cannot stop reading: %s", uv_strerror(uv_last_error(ipc->loop)));
    }

    if(buf.base) {
      delete[] buf.base;
      buf.base = NULL;
    }

    uv_err_t err = uv_last_error(handle->loop);
    if(err.code != UV_EOF) {
      RX_ERROR("Unhandled error");
    }

    r = uv_shutdown(&ipc->shutdown_req, handle, client_ipc_on_shutdown);
    if(r) {
      RX_ERROR("Error shutting down client: %s // @TODO do we need to delete ClientIPC here?", uv_strerror(uv_last_error(ipc->loop)));
    }

    return;
  }
  
  

  if(buf.base) {

    if(ipc->cb_read) {
      ipc->cb_read(ipc, buf.base, nbytes, ipc->cb_user);
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
  RX_VERBOSE("CLOSED!!!!");
}

void client_ipc_on_write(uv_write_t* req, int status) {
  RX_VERBOSE("WRITTEN!");
  delete req;
  req = NULL;
}

// -----------------------------------------------

ClientIPC::ClientIPC()
  :loop(NULL)
  ,cb_con(NULL)
  ,cb_read(NULL)
  ,cb_user(NULL)
{

  loop = uv_loop_new();
  if(!loop) {
    RX_ERROR("Cannot create a new loop");
    ::exit(EXIT_FAILURE);
  }
  
  connect_req.data = this;
  shutdown_req.data = this;
}

ClientIPC::~ClientIPC() {
  cb_user = NULL;
  cb_read = NULL;
  cb_con = NULL;
  RX_ERROR("@TODO - cleanup");
}

bool ClientIPC::setup(std::string sockfile, bool datapath,
                      client_ipc_on_connected_cb conCB,  client_ipc_on_read_cb readCB, 
                      void* user)
{

  int r = uv_pipe_init(loop, &pipe, 0);
  if(r) {
    RX_ERROR("Error setting up pipe: %s", uv_strerror(uv_last_error(loop)));
    return false;
  }

  if(datapath) {
    sockfile = rx_to_data_path(sockfile);
  }

  cb_con = conCB;
  cb_read = readCB;
  cb_user = user;

  pipe.data = this;
  uv_pipe_connect(&connect_req, &pipe, sockfile.c_str(), client_ipc_on_connect);

  return true;
}

void ClientIPC::update() {
  uv_run(loop, UV_RUN_NOWAIT);
}

void ClientIPC::write(char* data, size_t nbytes) {
  // @todo - check if we're connected
  uv_buf_t buf = uv_buf_init(data, nbytes);
  uv_write_t* req = new uv_write_t();
  req->data = this;
  
  int r = uv_write(req, (uv_stream_t*)&pipe, &buf, 1, client_ipc_on_write);
  if(r) {
    RX_ERROR("Error writing: %s", uv_strerror(uv_last_error(loop)));
  }
}
