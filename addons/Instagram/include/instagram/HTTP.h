#ifndef ROXLU_INSTAGRAM_HTTPH
#define ROXLU_INSTAGRAM_HTTPH

//extern "C" {
#include <uv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
//}

#include <map>
#include <string>
#include <sstream>
#include <instagram/SSLBuffer.h>

namespace roxlu {
struct HTTPParam {
  HTTPParam(std::string n, std::string v);
  HTTPParam();
  std::string name;
  std::string value;
};

class HTTPParams {
 public:
  HTTPParams();
  ~HTTPParams();
  void add(std::string name, std::string value);
  void percentEncode();
  std::string percentEncodeString(std::string v);
  std::string getHeaderString();
  std::string getQueryString();
  std::map<std::string, HTTPParam> entries;
 private:
  char rfc3986[256];
};


void ssl_write_to_socket(const char* data, size_t len, void* user);
void ssl_read_decrypted(const char* data, size_t len, void* user);

void httpreq_on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res);
void httpreq_on_connect(uv_connect_t* con, int status);
void httpreq_on_read(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf);
uv_buf_t httpreq_alloc(uv_handle_t* buffer, size_t suggestedSize); 


class HTTPRequest {
 public:
  HTTPRequest(uv_loop_t* loop, SSL_CTX* ssl_ctx = NULL);
  ~HTTPRequest();
  void setURL(const std::string host, const std::string path, bool isPost = false, bool isSecure = false);
  void addHeader(const std::string name, const std::string value);
  void addFormField(const std::string name, const std::string value);
  std::string getAsString();
  bool send();
  void sendData(const char* data, size_t len);
 public:
  HTTPParams fields;
  HTTPParams header;
  bool is_post;
  bool is_secure;
  std::string host;
  std::string path;
  uv_loop_t* loop;
  uv_connect_t connect_req;
  uv_tcp_t tcp_req;
  uv_write_t write_req;
  uv_getaddrinfo_t resolver_req;
  SSL_CTX* ssl_ctx;
  SSL* ssl;
  SSLBuffer ssl_buffer;
  std::vector<char> out_buffer;
};

/*
class HTTPConnection {
 public:
  HTTPConnection(uv_loopt_t* loop);
  ~HTTPConnection();
  void send(HTTPRequest& req);
 private:
  uv_loop_t* loop;
  uv_connect_t connect_req;
  uv_tcp_t tcp_req;
  uv_write_t write_req;
  uv_getaddrinfo_t resolver_req;
  SSL* ssl;
  SSLBuffer ssl_buffer;
};
*/
} // roxlu
#endif
