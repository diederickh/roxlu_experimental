#ifndef ROXLU_INSTAGRAMH
#define ROXLU_INSTAGRAMH

extern "C" {
#include <stdio.h>
#include <assert.h>
#include <uv.h>
}

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <instagram/SSLBuffer.h>
#include <instagram/HTTP.h>
#include <instagram/OAuth.h>
#include <roxlu/Roxlu.h> // for slre


namespace roxlu {

  uv_buf_t instagram_alloc_buffer(uv_handle_t* buffer, size_t suggestedSize);
  void instagram_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
  void instagram_on_new_connection(uv_stream_t* server, int status);
  void instagram_after_write(uv_write_t* req, int status);

  void dummy_ssl_info_callback(const SSL* ssl, int where, int ret);
  int instagram_ssl_verify_peer(int ok, X509_STORE_CTX* store);
  void instagram_http_request_close(HTTPRequest* req, void* user); // gets called when the HTTP connection is closed

  typedef void (*cb_instagram_on_data)(const char* data, size_t len, void* user); // called whenever we receive data on the socket for an endpoint
  typedef void (*cb_instagram_on_close)(const char* data, size_t len, void* user); // called when the request to an API endpoint is closed

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
  };

  struct InstagramAPICallback {
  InstagramAPICallback(cb_instagram_on_close closeCB, void* user, Instagram* instagram)
  :close_cb(closeCB)
  ,user(user)
      ,instagram(instagram)
    {
    }
    void* user;
    cb_instagram_on_close close_cb;
    Instagram* instagram;
  };

  class Instagram {
  public:
    Instagram();
    ~Instagram();

    bool setup(const std::string clientID, const std::string clientSecret, const std::string sslPrivateKeyFile);
    void requestAccessToken();
    void setAccessToken(const std::string token);
    void update();
    void subscribe(const std::string object, const std::string objectID, const std::string aspect, const std::string token);
    void subscriptions(cb_instagram_on_data dataCB = NULL, cb_instagram_on_close closeCB = NULL, void* user = NULL);
    void mediaPopular(cb_instagram_on_data dataCB = NULL, cb_instagram_on_close closeCB = NULL, void* user = NULL);
    void mediaSearch(HTTPParams& params, cb_instagram_on_data dataCB = NULL, cb_instagram_on_close closeCB = NULL, void* user = NULL);
    void locationsSearch(HTTPParams& params, cb_instagram_on_data dataCB = NULL, cb_instagram_on_close closeCB = NULL, void* user = NULL);
  public:
    bool is_setup;
    std::string client_id;
    std::string client_secret;
    uv_tcp_t server;
    uv_loop_t* loop;
    uv_idle_t idler;
    std::vector<InstagramConnection*> clients;
    std::vector<HTTPRequest*> requests;
    std::vector<InstagramAPICallback*> api_callbacks;
    bool must_stop_idler;
    SSLBuffer ssl_buffer;
    SSL_CTX* ssl_ctx;
    OAuth oauth;
  };

}
#endif
