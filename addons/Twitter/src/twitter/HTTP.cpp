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
    printf("JOINING HEADERS: %s - %s\n", h.name.c_str(), h.value.c_str());
  }
  return result;
}


// HTTP CONNECTION
HTTPConnection::HTTPConnection(HTTP* h)
  :state(0)
  ,bev(NULL)
  ,http(h)
  ,close_callback(NULL)
  ,close_callback_data(NULL)
  ,error_callback(NULL)
  ,error_callback_data(NULL)
  ,read_callback(NULL)
  ,read_callback_data(NULL)
{
}

HTTPConnection::~HTTPConnection() {
  printf("~HTTPConnection()\n");
}

void HTTPConnection::addToOutputBuffer(const char* data, size_t len) {
  evbuffer_add(bufferevent_get_output(bev), data, len);
}

void HTTPConnection::addToOutputBuffer(const std::string str) {
  addToOutputBuffer(str.c_str(), str.size());
}

// HTTP CONTEXT
// ------------
HTTP::HTTP() 
  :evbase(NULL)
  ,dnsbase(NULL)
{
  event_enable_debug_mode();
  evbase = event_base_new();
  dnsbase = evdns_base_new(evbase, 1);
  event_set_log_callback(HTTP::callbackLog);
}

HTTP::~HTTP() {
  removeAllConnections();
}

void HTTP::update() {
  event_base_loop(evbase, EVLOOP_NONBLOCK);
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
  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  c->bev = bufferevent_socket_new(evbase, -1, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(c->bev, HTTP::callbackRead, NULL, HTTP::callbackEvent, c);
  bufferevent_enable(c->bev, EV_READ | EV_WRITE);
  bufferevent_socket_connect_hostname(c->bev, dnsbase, AF_INET, r.getURL().host.c_str(), 80);

  c->addToOutputBuffer(r.makeRequestString());
  connections.push_back(c);
  printf("HTTP::sendRequest()\n");
  printf("\t%s\n", r.getURL().host.c_str());
  return c;
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
  //return sendRequest(r, readCB, readData, closeCB, closeData, errorCB, errorData);

  HTTPConnection* c = newConnection(readCB, readData, closeCB, closeData, errorCB, errorData);
  SSL* ssl = SSL_new(ctx);
  c->bev = bufferevent_openssl_socket_new(evbase, -1, ssl, BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(c->bev, HTTP::callbackRead, NULL, HTTP::callbackEvent, c);

  int rc = bufferevent_socket_connect_hostname(c->bev, dnsbase, AF_INET, r.getURL().host.c_str(), 443);
  bufferevent_enable(c->bev, EV_READ | EV_WRITE);
  c->addToOutputBuffer(r.makeRequestString());
  connections.push_back(c);
}


HTTPConnection* HTTP::testSecure(SSL_CTX* sslContext) {
  printf("testSecure()\n");
  printf("---------------------\n");

  //https://www.ssllabs.com/ssltest/
  HTTPRequest r;
  //  r.setURL(HTTPURL("https", "www.ssllabs.com", "/ssltest/"));
  r.setURL(HTTPURL("https", "developer.mozilla.org", "/en-US/"));
  

  SSL* ssl = SSL_new(sslContext);
  HTTPConnection* c = new HTTPConnection(this);
  c->bev = bufferevent_openssl_socket_new(evbase, -1, ssl, BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(c->bev, HTTP::callbackRead, NULL, HTTP::callbackEvent, c);

  //  int rc = bufferevent_socket_connect_hostname(c->bev, dnsbase, AF_INET, "stream.twitter.com", 443);
  int rc = bufferevent_socket_connect_hostname(c->bev, dnsbase, AF_INET, r.getURL().host.c_str(), 443);
  if(rc < 0) {
    printf("Cannot connect .. : %s\n", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
  }
  c->addToOutputBuffer(r.makeRequestString());
  bufferevent_enable(c->bev, EV_READ | EV_WRITE);
}

// - END TEST
/*
void HTTP::callbackError(bufferevent* bev, short what, void* ctx) {
  printf("HTTP::callbackError()\n");
  if(what & BEV_EVENT_EOF) {
    printf("\tBEV_EVENT_EOF\n");
  }
  if(what & BEV_EVENT_ERROR) {
    printf("\tBEV_EVENT_ERROR\n");
  }
  if(what & BEV_EVENT_TIMEOUT) {
    printf("\tBEV_EVENT_TIMEOUT\n");
  }
  if(what & BEV_EVENT_CONNECTED) {
    printf("\tBEV_EVENT_CONNECTED\n");
  }
  if(what & BEV_EVENT_READING) {
    printf("\tBEV_EVENT_READING\n");
  }
  if(what & BEV_EVENT_WRITING) {
    printf("\tBEV_EVENT_WRITING\n");
  }
}
*/

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

void HTTP::callbackRead(bufferevent* bev, void* ctx) {
  printf("HTTP::callbackRead()\n");
  SSL* ssl = bufferevent_openssl_get_ssl(bev);
  if(!ssl) {
    printf("NO SSL!\n");
  }
  else {
    printf("\t SSL: %p\n", ssl);
    printf("\t SSL State: %s\n", SSL_state_string(ssl));
    printf("\t SSL State Long: %s\n", SSL_state_string_long(ssl));
  }
  HTTPConnection* c = static_cast<HTTPConnection*>(ctx);
  char tmp[1024];
  int n;
  struct evbuffer *input = bufferevent_get_input(c->bev);
  while ((n = evbuffer_remove(input, tmp, sizeof(tmp))) > 0) {
    c->buffer.addBytes(tmp, n);
  }
  c->buffer.print();
  c->response.parseResponse(c->buffer);

  if(c->read_callback) {
    c->read_callback(c, c->read_callback_data);
  }
}

void HTTP::callbackEvent(bufferevent* bev, short events, void* ctx) {
  printf("HTTP::callbackEvent()\n");

  // CONNECTED
  if(events & BEV_EVENT_CONNECTED) {
    printf("\t>BEV_EVENT_CONNECTED\n");
  }
  // ERROR
  else if(events & BEV_EVENT_ERROR) {
    HTTPConnection* c = static_cast<HTTPConnection*>(ctx);
    int err = bufferevent_socket_get_dns_error(bev);
    printf("\t>BEV_EVENT_ERROR = %d\n", err);
    if(err) {
      printf("# ERROR: DNS: %s\n", evutil_gai_strerror(err));
    }
    if(c->error_callback) {
      c->error_callback(c, c->error_callback_data);
    }
    bufferevent_free(c->bev);
    c->bev = NULL;
    c->http->removeConnection(c);
  }
  // DISCONNECTED
  else if(events & BEV_EVENT_EOF) {
    printf("\t>BEV_EVENT_EOF\n");
    HTTPConnection* c = static_cast<HTTPConnection*>(ctx);
    if(c->close_callback) {
      c->close_callback(c, c->close_callback_data);
    }
    bufferevent_free(c->bev);
    c->bev = NULL;
    c->http->removeConnection(c);
  }
}

void HTTP::callbackLog(int severity, const char* msg) {
  printf("LOG: %d, %s\n", severity, msg);
}

void HTTP::removeAllConnections() {
   for(std::vector<HTTPConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
     HTTPConnection* c = *it;
     if(c->bev != NULL) {
       bufferevent_free(c->bev);
       c->bev = NULL;
     }
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


