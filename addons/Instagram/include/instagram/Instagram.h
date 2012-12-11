#ifndef ROXLU_INSTAGRAMH
#define ROXLU_INSTAGRAMH

extern "C" {
#include <stdio.h>
#include <assert.h>
#include <uv.h>
#include <curl/curl.h>
#include <pcre.h>
}

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <instagram/SSLBuffer.h>
#include <instagram/HTTP.h>
#include <roxlu/Roxlu.h> // for slre

#define CHECK_CURL_RES(code, msg) { if(code != CURLE_OK) { printf(msg); printf("ERROR: %s\n", curl_easy_strerror(code)); } }

namespace roxlu {

  uv_buf_t instagram_alloc_buffer(uv_handle_t* buffer, size_t suggestedSize);
  void instagram_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
  void instagram_on_new_connection(uv_stream_t* server, int status);
  void instagram_after_write(uv_write_t* req, int status);
  void instagram_idle_watch(uv_idle_t* handle, int status);

  //  void instagram_ssl_write_to_socket(const char* data, size_t len, void* insta);
  // void instagram_ssl_read_decrypted(const char* data, size_t len, void* insta);
  void dummy_ssl_info_callback(const SSL* ssl, int where, int ret);
  int instagram_ssl_verify_peer(int ok, X509_STORE_CTX* store);

  class Instagram;
  
  enum InstagramConnectionStates {
    ICS_PARSE_HUB_CHALLENGE,
    ICS_NONE
  };

  struct InstagramConnection {
    InstagramConnection(Instagram* instagram);
    ~InstagramConnection();
    void send(const char* data, size_t len);

    uv_write_t write_req;
    uv_tcp_t* client;
    Instagram* instagram;
    std::string buffer;
    int state;
    CURL* curl; // not used!
  };

  class Instagram {
  public:
    Instagram();
    ~Instagram();
    bool setup(const std::string clientID, const std::string clientSecret, const std::string sslPrivateKeyFile);
    void update();
    void subscribe(const std::string object, const std::string objectID, const std::string aspect, const std::string token);
    void sendHTTP(const std::string data);

  public:
    std::string client_id;
    std::string client_secret;
    uv_tcp_t server;
    uv_loop_t* loop;
    uv_idle_t idler;
    std::vector<InstagramConnection*> clients;
    bool must_stop_idler;
    CURLM* curlm;
    std::vector<CURL*> curls;
    SSLBuffer ssl_buffer;
    SSL_CTX* ssl_ctx;

  };

}
#endif
