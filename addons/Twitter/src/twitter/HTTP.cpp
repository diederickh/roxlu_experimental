#include <twitter/HTTP.h>

HTTPRequest::HTTPRequest(int method)
  :method(method)
  ,version(HTTP10)
  ,parse_state(HPS_METHOD)
  ,body_start_dx(0)
{
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

  // BODY
  std::string body;
  if(method == REQUEST_POST) {
    parameters.percentEncode(); 
    body = parameters.getQueryString();
  }

  // HEADERS
  std::string req = makeHTTPString();
  addHeader(HTTPHeader("Host", url.host));
  addHeader(HTTPHeader("Content-Length",body.size()));

  if(method == REQUEST_POST) {
    addHeader(HTTPHeader("Content-Type","application/x-www-form-urlencoded"));
  }
  
  // COMBINE HEADER + BODY
  req += headers.join();
  req +="\r\n"; // end of headers
  req += body;

  printf("%s\n", req.c_str());
  
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

bool HTTPRequest::parseResponse(Buffer& b) {
  // We only parse till the body
  if(parse_state == HPS_BODY) {
    return true;
  }

  while(true) {
    size_t size = b.getSize();
    switch(parse_state) {
      
      // EXTRACT VERSION + RESPONSE CODE
    case HPS_METHOD: {
      if(size < 8) {
        return false;
      }
      if((b[0] == 'H' || b[0] == 'h') 
          && (b[1] == 'T' || b[1] == 't') 
          && (b[2] == 'T' || b[2] == 't')
          && (b[3] == 'P' || b[3] == 'p')  )
        {
          version = (b[7] == 1) ? HTTP11 : HTTP10;
          size_t vpos = b.findNextByte(9, ' ');
          response_code = atoi(std::string(&b[9], vpos).c_str());
          parse_state = HPS_HEADERS; // continue with headers.
          for(size_t i = (9+vpos); i < size-1; ++i) {
            if(b[i] == '\r' && b[i+1] == '\n') {
              b.dx = i+2;
              break;
            }
          }
          continue;
        }
      break;
    }
      
      // EXTRACT HEADERS
    case HPS_HEADERS: {
      HTTPHeader header;
      std::string value;
      std::string name;
      int n = 0;
      for(size_t i = b.dx; i < size; ++i) {
        if(i < size-4 && b[i] == '\r' && b[i+1] == '\n' && b[i+2] == '\r' && b[i+3] == '\n') { // all headers done
          parse_state = HPS_BODY;
          b.dx = i+4;
          body_start_dx = b.dx;
          return true; // after finding the body we don't parse the buffer anymore
        }
        if(b[i] == '\r' && ((i+1) < size) && b[i+1] == '\n') {
          std::transform(header.name.begin(), header.name.end(), header.name.begin(), ::tolower);
          addHeader(header);
          header.name.clear();
          header.value.clear();
          n = 0;
          i = i + 1;
        }
        else {
          if(n == 0 && b[i] == ':') {
            n = 1; 
            i++;
            continue;
          }
          if(n == 0) {
            header.name.push_back(b[i]);
          }
          else {
            header.value.push_back(b[i]);
          }
        }
      }
      break;
    };

      // WE ARE AT THE BODY.. DON'T DO ANYTHING MORE
    case HPS_BODY:{
       return true;
    }

    default:printf("ERROR: Unhandled http parse state.\n"); break;
    }
  }
  return false; 
}

std::string HTTPRequest::getBody(Buffer& b) {
  std::string body;
  for(size_t i = b.dx; i < b.getSize(); ++i) {
    body.push_back(b[i]);
    b.dx = i;
  }
  return body;
}

/*
bool HTTPRequest::hasCompleteBody(Buffer& b) {
  size_t body_len = b.getSize() - body_start_dx;
  print("We have: %zu bytes as body len; header tells us: %s\n", 
        return true;
}
*/

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

/*
HTTPHeader::HTTPHeader(std::string name, std::string value)
  :name(name)
  ,value(value)
{
  printf("HEADER: VALUE: %s\n", value.c_str());
}
*/

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
#ifndef USE_LIBUV
  ,bev(NULL)
#endif
  ,http(h)
  ,close_callback(NULL)
  ,close_callback_data(NULL)
  ,error_callback(NULL)
  ,error_callback_data(NULL)
  ,read_callback(NULL)
  ,read_callback_data(NULL)
#ifdef USE_OPENSSL
  ,ssl(NULL)
#endif
{
#ifdef USE_LIBUV
  resolver_req.data = this;
  connect_req.data = this;
  socket.data = this;
  write_req.data = this;
#endif
}

HTTPConnection::~HTTPConnection() {
  printf("~HTTPConnection()\n");
}

void HTTPConnection::addToOutputBuffer(const char* data, size_t len) {
  printf("\nHTTPConnection::addToOutputBuffer()\n");
  uv_buf_t buf;
  buf.base = (char*)data;
  buf.len = len;
  int result = uv_write(&write_req, (uv_stream_t*)&socket, &buf, 1, HTTP::callbackOnWritten);
}

void HTTPConnection::addToOutputBuffer(const std::string str) {
  addToOutputBuffer(str.c_str(), str.size());
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
#ifndef USE_LIBUV
  event_base_loop(evbase, EVLOOP_NONBLOCK);
#else 
   uv_run_once(loop);
#endif
}

HTTPConnection* HTTP::sendRequest(
                       HTTPRequest& r
                       ,http_cb_on_read readCB 
                       ,void* readData 
                       ,http_cb_on_close closeCB
                       ,void* closeData
                       ,http_cb_on_error errorCB
                       ,void* errorData
)
{
  /*
  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  std::string http_req = r.makeRequestString();
  c->buffer_out.addBytes(http_req.c_str(), http_req.size());

  connections.push_back(c);

  struct addrinfo hints;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
  //  uv_getaddrinfo_t resolver; // @todo check if this needs to be created on the heap
  printf("uloop: %p\n", loop);
  int result = uv_getaddrinfo(loop, &c->resolver_req, HTTP::callbackOnResolved, r.getURL().host.c_str(), "80", &hints);
  if(result) {
    printf("ERROR: HTTP::sendRequest(), error uv_getaddrinfo: %s\n", uv_err_name(uv_last_error(loop)));
    return NULL;
  }
  return c;
  */
}


// TESTING WITH HTTPS
HTTPConnection* HTTP::sendSecureRequest(
                                        SSL_CTX* ctx
                                        ,HTTPRequest& r
                                        ,http_cb_on_read readCB
                                        ,void* readData 
                                        ,http_cb_on_close closeCB
                                        ,void* closeData 
                                        ,http_cb_on_error errorCB 
                                        ,void* errorData 
                                        )
{
  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  std::string http_req = r.makeRequestString();
  c->ssl_buffer.addApplicationData(http_req);
  c->ssl = SSL_new(ctx);

  struct addrinfo hints;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;

  printf("> Starting up DNS resolve: %p\n", loop);
  int result = uv_getaddrinfo(loop, &c->resolver_req, HTTP::callbackOnResolved, r.getURL().host.c_str(), "443", &hints);
  if(result) {
    printf("ERROR: HTTP::sendRequest(), error uv_getaddrinfo: %s\n", uv_err_name(uv_last_error(loop)));
    return NULL;
  }
  connections.push_back(c);
  return c;
}

HTTPConnection* HTTP::newConnection(    
                                    http_cb_on_read readCB
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
  c->read_callback = readCB;
  c->read_callback_data = readData;
  return c;
}

// ON RESOLVED
// ----------
void HTTP::callbackOnResolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
  if(status == -1) {
    printf("ERROR: callbackOnResolved(). @todo remove from connections.\n");
    return;
  }

  char addr[17] = {'\0'};
  uv_ip4_name((struct sockaddr_in*)res->ai_addr, addr, 16);
  printf("Address: %s\n", addr);

  HTTPConnection* c = static_cast<HTTPConnection*>(resolver->data);
  uv_tcp_init(resolver->loop, &c->socket);
  uv_tcp_connect(&c->connect_req, &c->socket, *(struct sockaddr_in*)res->ai_addr, HTTP::callbackOnConnect);
  // @todo call uv_freeaddrinfo(res) here?
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
  int result = uv_read_start((uv_stream_t*)&c->socket, HTTP::callbackOnAlloc, HTTP::callbackOnRead);

  if(!c->ssl) {
    printf("ADD NORMAL HTTP CONNECTION!\n");
    //c->addToOutputBuffer(c->buffer_out.getPtr(), c->buffer_out.getSize());
  }
  else {
    SSL_set_connect_state(c->ssl);
    SSL_do_handshake(c->ssl);
    c->ssl_buffer.init(c->ssl, HTTP::callbackSSLMustWriteToSocket, c, HTTP::callbackSSLReadDecryptedData, c);
    c->ssl_buffer.update();
  }
}

void HTTP::callbackSSLMustWriteToSocket(const char* data, size_t len, void* con) {
  if(len <= 0) {
    return;
  }
  HTTPConnection* c = static_cast<HTTPConnection*>(con);
  uv_buf_t buf;
  buf.base = (char*)data;
  buf.len = len;
  int r = uv_write(&c->write_req, (uv_stream_t*)&c->socket, &buf, 1, NULL);
  if(r < 0) {
    printf("ERROR: callbackSSLMustWriteToSocket() error: %s\n", uv_err_name(uv_last_error(c->http->loop)));
  }
}
 
void HTTP::callbackSSLReadDecryptedData(const char* data, size_t len, void* con) {
  HTTPConnection* c = static_cast<HTTPConnection*>(con);
  printf("Read decrypted data: %zu bytes\n", len);
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
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
      c->buffer_in.print();
      if(c->close_callback) {
        c->close_callback(c, c->close_callback_data);
      }
    }
    printf("\t ERROR: error while reading, err: %s.\n", uv_strerror(err));
    return;
  }
  else {
    HTTPConnection* c = static_cast<HTTPConnection*>(tcp->data);
    c->ssl_buffer.addEncryptedData(buf.base, nread);
    c->ssl_buffer.update();
  }


  /*
  // NOTHING?
  if(nread == 0) {
    return;
  }

  // CONNECTION CLOSED
  if(nread < 0) {
    uv_err_t err = uv_last_error(tcp->loop);
    if(err.code == UV_EOF) {
      HTTPConnection* c = static_cast<HTTPConnection*>(tcp->data);
      c->buffer_in.print();
      if(c->close_callback) {
        c->close_callback(c, c->close_callback_data);
      }
    }
    printf("\t ERROR: error while reading, err: %s.\n", uv_strerror(err));
    return;
  }

  // HANDLE INCOMING DATA
  HTTPConnection* c = static_cast<HTTPConnection*>(tcp->data);
  if(c->ssl) {

  }
  else {
    c->buffer_in.addBytes(buf.base, nread);
    c->response.parseResponse(c->buffer_in);
    if(c->read_callback) {
      c->read_callback(c, c->read_callback_data);
    }
  }
  free(buf.base);
  */
}
 

// ON ALLOC
// ----------
uv_buf_t HTTP::callbackOnAlloc(uv_handle_t* con, size_t size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(size); // @todo there is probably a better way to deal with memory.. and where is this freed?
  buf.len = size;
  return buf;
}

void HTTP::callbackOnWritten(uv_write_t* req, int status) {
  HTTPConnection* c = static_cast<HTTPConnection*>(req->data);
  printf("WRITTEN: %d\n", status);
}




void HTTP::removeAllConnections() {
  printf("@todo HTTP::removeAllConnections()\n");
  /*
   for(std::vector<HTTPConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
     HTTPConnection* c = *it;
#ifndef USE_LIBUV
     if(c->bev != NULL) {
       bufferevent_free(c->bev);
       c->bev = NULL;
     }
#endif
     delete c;
   }
   connections.clear();
  */
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


