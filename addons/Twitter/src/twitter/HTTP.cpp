#include <twitter/HTTP.h>

HTTPRequest::HTTPRequest(int method)
  :method(method)
  ,version(HTTP10)
  ,callback_read_body(NULL)
  ,callback_read_body_data(NULL)
{
  parser_settings.on_url = HTTPRequest::parserCallbackOnURL;
  parser_settings.on_message_begin = HTTPRequest::parserCallbackOnMessageBegin;
  parser_settings.on_header_field = HTTPRequest::parserCallbackOnHeaderField;
  parser_settings.on_header_value = HTTPRequest::parserCallbackOnHeaderValue;
  parser_settings.on_headers_complete = HTTPRequest::parserCallbackOnHeadersComplete;
  parser_settings.on_body = HTTPRequest::parserCallbackOnBody;
  parser_settings.on_message_complete = HTTPRequest::parserCallbackOnMessageComplete;
  http_parser_init(&parser, HTTP_RESPONSE);
  parser.data = this;
}

HTTPRequest::~HTTPRequest() {
}

std::string HTTPRequest::makeHTTPString() {
  std::string req;
  if(method == REQUEST_GET) {
    req = "GET ";
  }
  else if(method == REQUEST_POST) {
    req = "POST ";
  }
  req += url.path +" HTTP/" +((version == HTTP10) ? "1.0" : "1.1") +"\r\n";
  return req;
}

std::string HTTPRequest::makeRequestString() {
  std::string body;
  if(method == REQUEST_POST) {
    parameters.percentEncode(); 
    body = parameters.getQueryString();
  }

  std::string req = makeHTTPString();
  addHeader(HTTPHeader("Host", url.host));
  addHeader(HTTPHeader("Content-Length",body.size()));

  if(method == REQUEST_POST) {
    addHeader(HTTPHeader("Content-Type","application/x-www-form-urlencoded"));
  }

  req += headers.join();
  req +="\r\n"; 
  req += body;

  //printf("%s\n", req.c_str());
  return req;
}

void HTTPRequest::addHeader(HTTPHeader h) {
  headers.add(h);
}

void HTTPRequest::addContentParameter(HTTPParameter p) {
  parameters.add(p);
}

void HTTPRequest::addQueryStringParameter(HTTPParameter p) {
  querystring.add(p);
}

void HTTPRequest::copyContentParameters(const HTTPParameters& p) {
  parameters.copy(p);
}

void HTTPRequest::addToInputBuffer(const char* data, size_t len) { 
  buffer_in.addBytes(data, len);
}

bool HTTPRequest::parseInputBuffer() {  // Response(Buffer& b) { // const char* data, size_t len) { // Buffer& b) {
  size_t len = buffer_in.getNumBytesToRead();
  if(len <= 0) {
    return true;
  }

  int nparsed = http_parser_execute(&parser, &parser_settings, buffer_in.getReadPtr(), len);
  if(nparsed < 0) {
    return false;
  }

  buffer_in.dx += nparsed;
  return true;
}

int HTTPRequest::parserCallbackOnMessageBegin(http_parser* p) {
  return 0;
}

int HTTPRequest::parserCallbackOnURL(http_parser* p, const char* at, size_t len) {
  return 0;
}

int HTTPRequest::parserCallbackOnHeaderField(http_parser* p, const char* at, size_t len) { 
  return 0;
}

int HTTPRequest::parserCallbackOnHeaderValue(http_parser* p, const char* at, size_t len) {
  return 0;
}

int HTTPRequest::parserCallbackOnHeadersComplete(http_parser* p) {
  return 0;
}

int HTTPRequest::parserCallbackOnBody(http_parser* p, const char* at, size_t len) {
  HTTPRequest* r = static_cast<HTTPRequest*>(p->data);
  if(r->callback_read_body) {
    r->callback_read_body(at, len, r->callback_read_body_data);
  }
  r->buffer_in.flushReadBytes();
  return 0;
}

int HTTPRequest::parserCallbackOnMessageComplete(http_parser* p) { 
  return 0;
}

void HTTPRequest::setReadBodyCallback(cb_request_read_body readCB, void* readData) {
  callback_read_body = readCB;
  callback_read_body_data = readData;
}

// HTTP PARAMETER
// --------------
HTTPParameter::HTTPParameter() {
}

HTTPParameter::HTTPParameter(const std::string name, const std::string value) 
  :name(name)
  ,value(value)
{
}

void HTTPParameter::print() {
  printf("%s = %s\n", name.c_str(), value.c_str());
}

// HTTP PARAMETERS
// ---------------
void HTTPParameters::add(const HTTPParameter p) {
  entries.insert(std::pair<std::string, HTTPParameter>(p.name, p));
}

void HTTPParameters::copy(const HTTPParameters& other) {
  entries.insert(other.entries.begin(), other.entries.end());
}

void HTTPParameters::print() {
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    it->second.print();
  }
}

void HTTPParameters::clear() {
  entries.clear();
}

void HTTPParameters::percentEncode() {
  PercentEncode enc;
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPParameter& p = it->second;
    p.name = enc.encode(p.name);
    p.value = enc.encode(p.value);
  }
}

std::string HTTPParameters::getQueryString() {
  std::string qs;
  size_t len = entries.size();
  size_t i = 0;
  for(std::map<std::string, HTTPParameter>::iterator it = entries.begin(); it != entries.end(); ++it) {
    qs += it->second.name +"=" +it->second.value;
    if(i < len-1) {
      qs += "&";
    }
    ++i;
  }
  return qs;
}

bool HTTPParameters::exist(const std::string name) {
  std::map<std::string, HTTPParameter>::iterator it = entries.find(name);
  return (it != entries.end());
}

HTTPParameter& HTTPParameters::operator[](const std::string name) {
  return entries[name];
}

size_t HTTPParameters::createFromVariableString(std::string str) {
  int s = 0;
  HTTPParameter p;
  for(int i = 0; i < str.size(); ++i) {
    if(str[i] == '=') {
      s = 1;
      continue;
    }
    else if(str[i] == '&') {
      add(p);
      p.name.clear();
      p.value.clear();
      s = 0;
      continue;
    }
    if(s == 0) {
      p.name.push_back(str[i]);
    }
    else if(s == 1) {
      p.value.push_back(str[i]);
    }
  }
  if(p.name.size()) {
   add(p);
  }
  return entries.size();
}


// HTTPURL
// --------
HTTPURL::HTTPURL(std::string proto, std::string host, std::string path)
  :proto(proto) // e.g.: http or https
  ,host(host) // e.g.: api.twitter.com
  ,path(path) // e.g.: /oauth/request_token
{
}

HTTPURL::HTTPURL() {
}

std::string HTTPURL::getString() {
  return proto +"://" +host +path;
}

// HTTP HEADER
// -----------
HTTPHeader::HTTPHeader() {
}

// HTTP HEADERS
// -------------
void HTTPHeaders::add(const std::string name, const std::string value) {
  HTTPHeader h(name, value);
  add(h);
}

void HTTPHeaders::add(const HTTPHeader h) {
  entries.insert(std::pair<std::string, HTTPHeader>(h.name, h));
}

std::string HTTPHeaders::join(std::string nameValueSep, std::string lineEnd) {
  std::string result;
  for(std::map<std::string, HTTPHeader>::iterator it = entries.begin(); it != entries.end(); ++it) {
    HTTPHeader& h = it->second;
    result += h.name +nameValueSep +h.value +lineEnd;
  }
  return result;
}

// HTTP CONNECTION
HTTPConnection::HTTPConnection(HTTP* h)
  :state(0)
  ,http(h)
  ,close_callback(NULL)
  ,close_callback_data(NULL)
  ,error_callback(NULL)
  ,error_callback_data(NULL)
  ,ssl(NULL)
{
  resolver_req.data = this;
  connect_req.data = this;
  socket.data = this;
  write_req.data = this;
}

HTTPConnection::~HTTPConnection() {
  printf("~HTTPConnection()\n");
}

void HTTPConnection::addToOutputBuffer(const char* data, size_t len) {
  buffer_out.addBytes(data, len);
}

void HTTPConnection::addToOutputBuffer(const std::string str) {
  addToOutputBuffer(str.c_str(), str.size());
}

void HTTPConnection::send(const char* data, size_t len) {
  if(len <= 0) {
    return;
  }

  uv_buf_t buf;
  buf.base = (char*)data;
  buf.len = len;

  int r = uv_write(&write_req, (uv_stream_t*)&socket, &buf, 1, NULL); // HTTP::callbackOnWritten);
  if(r < 0) {
    printf("ERROR: callbackSSLMustWriteToSocket() error: %s\n", uv_err_name(uv_last_error(http->loop)));
  }
}

// HTTP CONTEXT
// ------------
HTTP::HTTP() 
   :loop(NULL)
{
  loop = uv_loop_new();
  loop->data = this;
}

HTTP::~HTTP() {
  removeAllConnections();
}

void HTTP::update() {
   uv_run_once(loop);
}

HTTPConnection* HTTP::sendRequest(
                       HTTPRequest& r
                       ,cb_request_read_body readCB 
                       ,void* readData 
                       ,http_cb_on_close closeCB
                       ,void* closeData
                       ,http_cb_on_error errorCB
                       ,void* errorData
)
{
  
  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  c->addToOutputBuffer(r.makeRequestString());
  connections.push_back(c);

  struct addrinfo hints;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
  
  int result = uv_getaddrinfo(loop, &c->resolver_req, HTTP::callbackOnResolved, r.getURL().host.c_str(), "80", &hints);
  if(result) {
    printf("ERROR: HTTP::sendRequest(), error uv_getaddrinfo: %s\n", uv_err_name(uv_last_error(loop)));
    return NULL;
  }
  return c;
}


// HTTPS
HTTPConnection* HTTP::sendSecureRequest(
                                        SSL_CTX* ctx
                                        ,HTTPRequest& r
                                        ,cb_request_read_body readCB
                                        ,void* readData 
                                        ,http_cb_on_close closeCB
                                        ,void* closeData 
                                        ,http_cb_on_error errorCB 
                                        ,void* errorData 
                                        )
{
  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  connections.push_back(c);
  std::string http_req = r.makeRequestString();
  c->ssl_buffer.addApplicationData(http_req);
  c->ssl = SSL_new(ctx);

  struct addrinfo hints;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  int result = uv_getaddrinfo(
                              loop, 
                              &c->resolver_req, 
                              HTTP::callbackOnResolved, 
                              r.getURL().host.c_str(), 
                              "443",
                              &hints
                              );
  if(result) {
    printf("ERROR: HTTP::sendRequest(), error uv_getaddrinfo: %s\n", uv_err_name(uv_last_error(loop)));
    return NULL;
  }

  return c;
}

HTTPConnection* HTTP::newConnection(    
                                    cb_request_read_body readCB
                                    ,void* readData 
                                    ,http_cb_on_close closeCB 
                                    ,void* closeData
                                    ,http_cb_on_error errorCB
                                    ,void* errorData 
                                   )
{
  HTTPConnection* c = new HTTPConnection(this);
  c->close_callback = closeCB;
  c->close_callback_data = closeData;
  c->error_callback = errorCB;
  c->error_callback_data = errorData;
  c->response.setReadBodyCallback(readCB, readData);
  return c;
}

// ON RESOLVED
// -----------
void HTTP::callbackOnResolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
  if(status == -1) {
    printf("ERROR: callbackOnResolved(). @todo remove from connections.\n");
    return;
  }

  HTTPConnection* c = static_cast<HTTPConnection*>(resolver->data);
  uv_tcp_init(resolver->loop, &c->socket);
  uv_tcp_connect(
                 &c->connect_req, 
                 &c->socket, 
                 *(struct sockaddr_in*)res->ai_addr, 
                 HTTP::callbackOnConnect
                 );
  uv_freeaddrinfo(res);
}

// ON CONNECT
// ----------
void HTTP::callbackOnConnect(uv_connect_t* con, int status) {
  if(status == -1) {
    printf("ERROR: callbackOnConnect(). @todo remove from connections\n");
    return;
  }

  HTTPConnection* c = static_cast<HTTPConnection*>(con->data);
  int result = uv_read_start(
                             (uv_stream_t*)&c->socket, 
                             HTTP::callbackOnAlloc, 
                             HTTP::callbackOnRead
                             );

  if(!c->ssl) {
    c->send(c->buffer_out.getPtr(), c->buffer_out.getSize());
  }
  else {
    SSL_set_connect_state(c->ssl);
    SSL_do_handshake(c->ssl);
    c->ssl_buffer.init(
                       c->ssl, 
                       HTTP::callbackSSLMustWriteToSocket, 
                       c, 
                       HTTP::callbackSSLReadDecryptedData, 
                       c
                       );
    c->ssl_buffer.update();
  }
}

void HTTP::callbackSSLMustWriteToSocket(const char* data, size_t len, void* con) {
  HTTPConnection* c = static_cast<HTTPConnection*>(con);
  c->send(data, len);
}
 
void HTTP::callbackSSLReadDecryptedData(const char* data, size_t len, void* con) {
  HTTPConnection* c = static_cast<HTTPConnection*>(con);
  c->response.addToInputBuffer(data, len);
  c->response.parseInputBuffer();
}

// ON READ
// ---------
void HTTP::callbackOnRead(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) {
  if(nread == 0) {
    return;
  }
  else if(nread < 0) {
    uv_err_t err = uv_last_error(tcp->loop);
    if(err.code == UV_EOF) {
      HTTPConnection* c = static_cast<HTTPConnection*>(tcp->data);
      if(c->close_callback) {
        c->close_callback(c, c->close_callback_data);
      }
      c->http->removeConnection(c);
    }
    else {
      printf("\t ERROR: error while reading, err: %s.\n", uv_strerror(err));
    }
    return;
  }
  else {
    HTTPConnection* c = static_cast<HTTPConnection*>(tcp->data);
    if(c->ssl) {
      printf("add encrypted data");      
      c->ssl_buffer.addEncryptedData(buf.base, nread);
      c->ssl_buffer.update();
      printf("@todo, does free(buf.base) cause segfaults?\n");
      free(buf.base);
    }
    else {
      c->response.addToInputBuffer(buf.base, nread);
      c->response.parseInputBuffer();
    }
  }
}
 

// ON ALLOC
// ----------
uv_buf_t HTTP::callbackOnAlloc(uv_handle_t* con, size_t size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(size); // @todo there is probably a better way to deal with memory.. and where is this freed?
  buf.len = size;
  return buf;
}

void HTTP::removeAllConnections() {
  for(std::vector<HTTPConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    HTTPConnection* c = *it;
    delete c;
  }
  connections.clear();
}

void HTTP::removeConnection(HTTPConnection* c) {
  for(std::vector<HTTPConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    if((*it) == c) {
      delete c;
      connections.erase(it);
      break;
    }
  }
}


