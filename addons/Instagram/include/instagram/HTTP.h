#ifndef ROXLU_INSTAGRAM_HTTPH
#define ROXLU_INSTAGRAM_HTTPH

#include <uv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

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
    void copy(const HTTPParams& p);
    std::string percentEncodeString(std::string v);
    std::string getHeaderString();
    std::string getQueryString();
    std::map<std::string, HTTPParam> entries;
  private:
    char rfc3986[256];
  };

  class HTTPRequest;

  void ssl_write_to_socket(const char* data, size_t len, void* user);
  void ssl_read_decrypted(const char* data, size_t len, void* user);

  void httpreq_on_resolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res);
  void httpreq_on_connect(uv_connect_t* con, int status);
  void httpreq_on_read(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf);
  uv_buf_t httpreq_alloc(uv_handle_t* buffer, size_t suggestedSize); 

  typedef void (*cb_on_http_data)(const char* data, size_t len, void* user);
  typedef void (*cb_on_http_close)(HTTPRequest* req, void* user);

  class HTTPRequest {
  public:
    HTTPRequest(
                uv_loop_t* loop, 
                SSL_CTX* ssl_ctx = NULL, 
                cb_on_http_data onDataCallback = NULL, 
                void* onDataUser = NULL,
                cb_on_http_close onCloseCallback = NULL,
                void* onCloseData = NULL
                );
    ~HTTPRequest();
    void setURL(const std::string host, const std::string path, bool isPost = false, bool isSecure = false);
    void addHeader(const std::string name, const std::string value);
    void addFormField(const std::string name, const std::string value);
    void addURLField(const std::string name, const std::string value);
    std::string getAsString();
    bool send();
    void sendData(const char* data, size_t len);
  public:
    HTTPParams form_fields;
    HTTPParams url_fields;
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
    std::string in_buffer; // all received data
    cb_on_http_data data_cb; // data callback
    cb_on_http_close close_cb; // close callback
    void* close_user; // passed to the close callback
    void* data_user; // passed to the callback as user data
  };
} // roxlu
#endif
