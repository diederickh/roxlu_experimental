#include <instagram/HTTP.h>

namespace roxlu {

  // ------------------------------------
  // HTTP PARAMS
  // ------------------------------------
  HTTPParam::HTTPParam(std::string n, std::string v)
    :name(n)
    ,value(v)
  {
  }

  HTTPParam::HTTPParam() {
  }

  HTTPParams::HTTPParams() {
    for(int i = 0; i < 256; ++i) {
      rfc3986[i] = isalnum(i) || i == '~'||i == '-'||i == '.'|| i == '_'  ? i : 0;
    }
  }

  HTTPParams::~HTTPParams() {
  }

  void HTTPParams::add(std::string name, std::string value) {
    entries.insert(std::pair<std::string, HTTPParam>(name, HTTPParam(name, value)));
  }

  void HTTPParams::copy(const HTTPParams& p) {
    for(std::map<std::string, HTTPParam>::const_iterator it = p.entries.begin(); it != p.entries.end(); ++it) {
      add(it->second.name, it->second.value);
    }
  }

  std::string HTTPParams::getHeaderString() {
    std::string res; 
    for(std::map<std::string, HTTPParam>::iterator it = entries.begin(); it != entries.end(); ++it) {
      res += it->first +": " +it->second.value +"\r\n";
    }
    return res;
  }

  std::string HTTPParams::getQueryString() {
    std::string res;
    size_t s = entries.size();
    size_t n = 0;
    for(std::map<std::string, HTTPParam>::iterator it = entries.begin(); it != entries.end(); ++it) {
      res += it->second.name +"=" +it->second.value;
      n++;
      if(n < s) {
        res +="&";
      }
    }
    return res;
  }

  void HTTPParams::percentEncode() {
    for(std::map<std::string, HTTPParam>::iterator it = entries.begin(); it != entries.end(); ++it) {
      it->second.name = percentEncodeString(it->second.name);
      it->second.value = percentEncodeString(it->second.value);
    }
  }
  
  std::string HTTPParams::percentEncodeString(std::string input) {
    std::string result;
    char buf[5];
    char encoded;
    for(int i = 0; i < input.size(); ++i) {
      char c = input[i];
      if(rfc3986[c]) {
        sprintf(&encoded, "%c", rfc3986[c]);
        result.append(&encoded,1);
      }
      else {
        sprintf(buf, "%%%02X", c);
        for(int j = 0; j < strlen(buf); ++j) {
          result.append(&buf[j], 1);
        }
      }
    }
    return result;
  }

  // ------------------------------------
  // HTTP REQUEST
  // ------------------------------------
  HTTPRequest::HTTPRequest(
                           uv_loop_t* loop, 
                           SSL_CTX* sslCTX, 
                           cb_on_http_data onDataCallback, 
                           void* onDataUser,
                           cb_on_http_close onCloseCallback,
                           void* onCloseUser
                           )
    :is_post(false)
    ,is_secure(false)
    ,ssl(NULL)
    ,ssl_ctx(sslCTX)
    ,loop(loop)
    ,data_cb(onDataCallback)
    ,data_user(onDataUser) // gets passed to your callback
    ,close_cb(onCloseCallback)
    ,close_user(onCloseUser)
  {
    connect_req.data = this;
    tcp_req.data = this;
    write_req.data = this;
    resolver_req.data = this;
  }

  HTTPRequest::~HTTPRequest() {
  }

  void HTTPRequest::setURL(const std::string host, const std::string path, bool isPost, bool isSecure) {
    is_secure = isSecure;
    is_post = isPost;
    this->path = path;
    this->host = host;
  }

  void HTTPRequest::addHeader(const std::string name, const std::string value) {
    header.add(name, value);
  }

  void HTTPRequest::addFormField(const std::string name, const std::string value) {
    form_fields.add(name, value);
  }

  void HTTPRequest::addURLField(const std::string name, const std::string value) {
    url_fields.add(name, value);
  }

  std::string HTTPRequest::getAsString() {
    std::stringstream ss;
    std::string body;
    std::string header_str = header.getHeaderString();

    // Form fields
    if(is_post) {
      form_fields.percentEncode();
      body = form_fields.getQueryString();
    }

    // Query string
    std::string url_path = path;
    url_fields.percentEncode();
    std::string url_qs = url_fields.getQueryString();
    if(url_qs.size()) {
      url_path += "?" +url_qs;
    }

    ss << ((is_post) ? "POST ": "GET ") << url_path << " HTTP/1.1\r\n"
       << "Host: " << host << "\r\n"
       << "Accept: */*\r\n"
       << "Connection: close\r\n"
       << "Content-Length: " << body.size() << "\r\n";
    
    if(is_post) {
      ss << "Content-Type: application/x-www-form-urlencoded\r\n";
    }

    // Extra headers
    if(header_str.size()) {
      ss << header_str;
    }
    ss << "\r\n";

    // Body
    if(is_post && body.size()) {
        ss << body;
    }
  
    return ss.str();
  }

  bool HTTPRequest::send() {
    // CREATE HTTP REQUEST
    std::string request = getAsString();
    std::copy(request.begin(), request.end(), std::back_inserter(out_buffer));
  
    // HINTS FOR SOCKET
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
  
    // SECURE CONNECTION?
    std::string port = "80";
    if(is_secure) {
      port = "443";
    }

    int r = uv_getaddrinfo(loop, &resolver_req, httpreq_on_resolved, host.c_str(), port.c_str(), &hints);
    if(r) {
      printf("ERROR: cannot resolve addres: %s\n", uv_err_name(uv_last_error(loop)));
      return false;
    }
    return true;
  }

  // Used internally to really transfer something over the socket.
  void HTTPRequest::sendData(const char* data, size_t len) {
    if(len <= 0) {
      return ;
    }
    uv_buf_t buf;
    buf.base = (char*)data;
    buf.len = len;
    int r = uv_write(&write_req, (uv_stream_t*)&tcp_req, &buf, 1, NULL);
    if(r < 0) {
      printf("ERROR: sendData error: %s\n", uv_err_name(uv_last_error(loop)));
    }
  }

  // -----------------------------------------------
  // UV AND SSL CALLBACKS
  // -----------------------------------------------
  void ssl_write_to_socket(const char* data, size_t len, void* user) {

    HTTPRequest* req = static_cast<HTTPRequest*>(user);
    req->sendData(data, len);
  }

  void ssl_read_decrypted(const char* data, size_t len, void* user) {
    HTTPRequest* req = static_cast<HTTPRequest*>(user);
    std::copy(data, data+len, std::back_inserter(req->in_buffer));

    if(req->data_cb) {
       req->data_cb(data, len, req->data_user);
    }
  }

  void httpreq_on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res) {
    if(status == -1) {
      printf("ERROR: httpreq_on_resolved(), cannot resoleve. @todo clean memory\n");
      return;
    }

    HTTPRequest* req = static_cast<HTTPRequest*>(resolver->data);
    uv_tcp_init(req->loop, &req->tcp_req);
    uv_tcp_connect(&req->connect_req, &req->tcp_req, *(struct sockaddr_in*)res->ai_addr, httpreq_on_connect);
    uv_freeaddrinfo(res);
  }

  void httpreq_on_connect(uv_connect_t* con, int status) {
    if(status == -1) {
      printf("ERROR: httpreq_on_connect, cannot connect. @todo clean memory\n");
      return;
    }

    HTTPRequest* req = static_cast<HTTPRequest*>(con->data);
    int result = uv_read_start((uv_stream_t*)&req->tcp_req, httpreq_alloc, httpreq_on_read);
    if(result == -1) {
      printf("ERROR: uv_read_start() error: %s\n", uv_err_name(uv_last_error(req->loop)));
      return;
    }

    if(!req->is_secure) {
      req->sendData(&req->out_buffer[0], req->out_buffer.size());
    }
    else {
      req->ssl = SSL_new(req->ssl_ctx);
      req->ssl_buffer.addApplicationData(&req->out_buffer[0],req->out_buffer.size());
      SSL_set_connect_state(req->ssl);    
      SSL_do_handshake(req->ssl);
      req->ssl_buffer.init(req->ssl, ssl_write_to_socket, req, ssl_read_decrypted, req);
      req->ssl_buffer.update();
    }
  }

  uv_buf_t httpreq_alloc(uv_handle_t* buffer, size_t suggestedSize) {
    return uv_buf_init((char*)malloc(suggestedSize), suggestedSize);
  }

  void httpreq_on_read(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) {
    HTTPRequest* req = static_cast<HTTPRequest*>(tcp->data);
    if(nread < 0) {
      if(req->close_cb) {
        req->close_cb(req, req->close_user);
      }
      free(buf.base);
      buf.base = NULL;
      return;
    }

    if(req->is_secure) {
      req->ssl_buffer.addEncryptedData(buf.base, nread);
      req->ssl_buffer.update();
      free(buf.base);
      buf.base = NULL;
    }
    else {
      printf("@TODO handle insecure data\n");
    }
  }

} // roxlu
