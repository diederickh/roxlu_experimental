#include <roxlu/core/Log.h>
#include <http/HTTPConnection.h>

//-------------------------------------------------------------------------------
int httpconnection_on_message_begin(http_parser* p) {
  return 0;
}

int httpconnection_on_status_complete(http_parser* p) {
  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  if(c->cb_event) {
    c->cb_event(c, HTTP_ON_STATUS, NULL, 0, c->cb_event_user);
  }  
  return 0;
}


int httpconnection_on_url(http_parser* p, const char* at, size_t len) {
  return 0;
}

int httpconnection_on_header_field(http_parser* p, const char* at, size_t len) {
  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  c->last_header.assign(at, len);
  return 0;
}

int httpconnection_on_header_value(http_parser* p, const char* at, size_t len) {
  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  std::string header_value(at, len);
  c->headers.add(HTTPHeader(c->last_header, header_value));
  return 0;
}

int httpconnection_on_headers_complete(http_parser* p) {

  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  if(c->cb_event) {
    c->cb_event(c, HTTP_ON_HEADERS, NULL, 0, c->cb_event_user);
  }
  
  return 0;
}

int httpconnection_on_body(http_parser* p, const char* at, size_t len) {

  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  if(c->cb_event) {
    c->cb_event(c, HTTP_ON_BODY, at, len, c->cb_event_user);
  }

  return 0;
}

int httpconnection_on_message_complete(http_parser* p) {
  HTTPConnection* c = static_cast<HTTPConnection*>(p->data);
  if(c->cb_event) {
    c->cb_event(c, HTTP_ON_COMPLETE, NULL, 0, c->cb_event_user);
  }
  
  return 0;
}


//-------------------------------------------------------------------------------

void httpconnection_on_flush_input_buffer(const char* data, size_t len, void* user) {
#if 1
  for(size_t i = 0; i < len; ++i) {
    printf("%c", data[i]);
  }
#endif

  HTTPConnection* c = static_cast<HTTPConnection*>(user);
  http_parser_execute(&c->parser, &c->parser_settings, data, len);
}

void httpconnection_on_flush_output_buffer(const char* data, size_t len, void* user) {
  HTTPConnection* c = static_cast<HTTPConnection*>(user);
  c->send((char*)data, len);
}

//-------------------------------------------------------------------------------

void httpconnection_on_resolved(uv_getaddrinfo_t* req, int status, struct addrinfo* res) {

  HTTPConnection* c = static_cast<HTTPConnection*>(req->data);
  if(status == -1) {
    RX_ERROR("> cannot revolve host: %s", c->host.c_str());
    return;
  }

  char ip[17] = {0};
  uv_ip4_name((struct sockaddr_in*)res->ai_addr, ip, 16);
 
  int r = uv_tcp_connect(&c->connect_req, c->sock, 
                         *(struct sockaddr_in*)res->ai_addr, 
                         httpconnection_on_connect);
 
  uv_freeaddrinfo(res);
}

void httpconnection_on_connect(uv_connect_t* req, int status) {
  HTTPConnection* c = static_cast<HTTPConnection*>(req->data);
  if(status == -1) {
    RX_ERROR("> cannot connect: %s:", uv_strerror(uv_last_error(c->loop)));
    RX_ERROR("@ todo should be `delete` the connection here?");
    return;
  }


  int r = uv_read_start((uv_stream_t*)c->sock, httpconnection_on_alloc, httpconnection_on_read);
  if(r) {
    RX_ERROR("> uv_read_start() failed: %s", uv_strerror(uv_last_error(c->loop)));
    RX_ERROR("@ todo should be `delete` the connection here?");
    return;
  }

  if(c->ssl) {
    SSL_set_connect_state(c->ssl);
    SSL_do_handshake(c->ssl);
    c->buffer->update();
   }

  // trigger the output buffer 
  c->buffer->flushOutputBuffer();
}

void httpconnection_on_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
  HTTPConnection* c = static_cast<HTTPConnection*>(handle->data);

  if(nread < 0) {
    int r = uv_read_stop(handle);
    if(r) {
      RX_ERROR("> error uv_read_stop: %s", uv_strerror(uv_last_error(handle->loop)));
      
    }
    if(buf.base) {
      delete buf.base;
      buf.base = NULL;
    }

    uv_err_t err = uv_last_error(handle->loop);
    if(err.code != UV_EOF) {
      RX_ERROR("> disconnected from server but not correctly: %s",uv_strerror(uv_last_error(handle->loop))) ;
    }

    r = uv_shutdown(&c->shutdown_req, handle, httpconnection_on_shutdown);
    if(r) {
      RX_ERROR("> error shutting down client: %s", uv_strerror(uv_last_error(handle->loop)));
      RX_ERROR("@ todo should be `delete` the connection here?");
    }

    return;
  }

  c->addToInputBuffer(buf.base, nread);

  if(buf.base) {
    delete[] buf.base;
    buf.base = NULL;
  }
}

void httpconnection_on_write(uv_write_t* req, int status) {
  delete req;
}

void httpconnection_on_shutdown(uv_shutdown_t* req, int status) {
  HTTPConnection* c = static_cast<HTTPConnection*>(req->data);
  uv_close((uv_handle_t*)c->sock, httpconnection_on_close);
}

void httpconnection_on_close(uv_handle_t* handle) {
  HTTPConnection* c = static_cast<HTTPConnection*>(handle->data);

  delete handle;
  handle = NULL;

  if(c->cb_event) {
    c->cb_event(c, HTTP_ON_CLOSED, NULL, 0, c->cb_event_user);
  }
  
  if(c->cb_close) {
    c->cb_close(c, HTTP_ON_CLOSED, NULL, 0, c->cb_close_user);
  }
}

uv_buf_t httpconnection_on_alloc(uv_handle_t* handle, size_t nbytes) {
  char* buf = new char[nbytes];
  return uv_buf_init(buf, nbytes);
}

//-------------------------------------------------------------------------------

HTTPConnection::HTTPConnection(uv_loop_t* loop, std::string host, std::string port, SSL* ssl) 
  :host(host)
  ,port(port)
  ,loop(loop)
  ,cb_event(NULL)
  ,cb_event_user(NULL)
  ,cb_close(NULL)
  ,cb_close_user(NULL)
  ,ssl(ssl)
  ,buffer(NULL)
{
  sock = new uv_tcp_t();
  sock->data = this;
  resolver_req.data = this;
  connect_req.data = this;
  shutdown_req.data = this;

  if(ssl) {
    buffer = new HTTPBufferSSL(httpconnection_on_flush_input_buffer,
                               httpconnection_on_flush_output_buffer, this, ssl);
  }
  else {
    buffer = new HTTPBuffer(httpconnection_on_flush_input_buffer, 
                            httpconnection_on_flush_output_buffer, this);
  }

  parser_settings.on_message_begin = httpconnection_on_message_begin;  
  parser_settings.on_url = httpconnection_on_url;
  parser_settings.on_status_complete = httpconnection_on_status_complete;
  parser_settings.on_header_field = httpconnection_on_header_field;
  parser_settings.on_header_value = httpconnection_on_header_value;
  parser_settings.on_headers_complete = httpconnection_on_headers_complete;
  parser_settings.on_body = httpconnection_on_body;
  parser_settings.on_message_complete = httpconnection_on_message_complete;
  
  http_parser_init(&parser, HTTP_RESPONSE);
  parser.data = this;
  
}

HTTPConnection::~HTTPConnection() {
  RX_VERBOSE("@todo shouldn't we delete the sock here?, chack httpconnection_on_close");
  RX_VERBOSE("@todo WHAT DO WE DO WITH THE SSL BUFFER?");
  RX_VERBOSE("@todo delete buffer object ");

  if(buffer) {
    delete buffer;
    buffer = NULL;
  }

  loop = NULL;
  cb_event = NULL;
  cb_event_user = NULL;
  cb_close = NULL;
  cb_close_user = NULL;

  host.clear();
  port.clear();

  if(ssl) {
    SSL_free(ssl);
  }
}

bool HTTPConnection::connect(httpconnection_event_callback eventCB,  /* gets called when a socket event occurs */
                             void* eventUser)                        /* gets passed into eventCB */

 {

  int r = uv_tcp_init(loop, sock);
  if(r) {
    RX_ERROR("Cannot init socket");
    return false;
  }

  cb_event = eventCB;
  cb_event_user = eventUser;

  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
 
  RX_VERBOSE("Connecting to: %s", host.c_str());
  r = uv_getaddrinfo(loop, &resolver_req, httpconnection_on_resolved, 
                     host.c_str(), port.c_str(), &hints);

  if(r) {
    RX_ERROR("cannot uv_tcp_init(): %s", uv_strerror(uv_last_error(loop)));
    return false;
  }
    
  return true;
}

void HTTPConnection::addToOutputBuffer(const std::string str) {
  addToOutputBuffer(str.c_str(), str.size());
}

void HTTPConnection::addToOutputBuffer(const char* data, size_t len) {
  buffer->addToOutputBuffer(data, len);
}

void HTTPConnection::addToInputBuffer(const std::string str) {
  addToInputBuffer(str.c_str(), str.size());
}

void HTTPConnection::addToInputBuffer(const char* data, size_t len) {
  buffer->addToInputBuffer(data, len);
}

bool HTTPConnection::send(char* data, size_t len) {
  uv_buf_t buf = uv_buf_init(data, len);
  uv_write_t* req = new uv_write_t();
  req->data = this;

  int r = uv_write(req, (uv_stream_t*)sock, &buf, 1, httpconnection_on_write);
  if(r) {
    RX_ERROR("uv_write() failed: %s", uv_strerror(uv_last_error(loop)));
    return false;
  }
  
  return true;
}

bool HTTPConnection::close() {
  if(!sock) {
    RX_ERROR("Cannot close a socket which haven't been opened yet");
    return false;
  }
  uv_shutdown(&shutdown_req, (uv_stream_t*)sock, httpconnection_on_shutdown);
  return true;
}
