#include <iostream>
#include <twitter/HTTP.h>
#include <twitter/Twitter.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>


#define WHERE_INFO(ssl, w, flag, msg) { \
    if(w & flag) { \
      printf("\t"); \
      printf(msg); \
      printf(" - %s ", SSL_state_string(ssl)); \
      printf(" - %s ", SSL_state_string_long(ssl)); \
      printf("\n"); \
    }\
 } 

void dummy_ssl_info_callback(const SSL* ssl, int where, int ret) {
  if(ret == 0) {
    printf("ssl error occured.\n");
    return;
  }

  WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
  WHERE_INFO(ssl, where, SSL_CB_EXIT, "EXIT");
  WHERE_INFO(ssl, where, SSL_CB_READ, "READ");
  WHERE_INFO(ssl, where, SSL_CB_WRITE, "WRITE");
  WHERE_INFO(ssl, where, SSL_CB_ALERT, "ALERT");
  WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
}

void dummy_ssl_msg_callback(int writep, int version, int contentType, const void* buf, size_t len, SSL* ssl, void *arg) {
  // printf("\t<Message callback with length: %zu>\n", len);
}

int dummy_ssl_verify_callback(int ok, X509_STORE_CTX* store) {
  return 1;
}

void on_status_update_callback(const char* data, size_t len, void* user) {
  printf("\n============================================\n");
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
  printf("\n============================================\n");
}

void on_filter_update_callback(const char* data, size_t len, void* user) {
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void on_access_token_received_callback(TwitterTokenInfo info, void* data) {
  printf("Store the following token and secret somewhere safe:\n");
  printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf("OAuth token: %s\n", info.oauth.token.c_str());
  printf("OAuth token secret: %s\n", info.oauth_token_secret.c_str());
  printf("User id: %s\n", info.user_id.c_str());
  printf("User name: %s\n", info.screen_name.c_str());
  printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void on_request_token_callback(std::string token, std::string tokenSecret, std::string redirectURL, void* data) {
  Twitter* t = static_cast<Twitter*>(data);
  printf("Request token: %s, token_secret: %s\n", token.c_str(), tokenSecret.c_str());
  std::string pin;
  printf("Open this URL: %s\npin:", redirectURL.c_str());
  std::cin >> pin;
  printf("Entered:%s\n", pin.c_str());
  t->exchangeRequestTokenForAccessToken(token, pin, on_access_token_received_callback, t);
  printf("called..\n");
}

int main() {
  const char* CLIENT_CERT_FILE = "/Users/diederickhuijbers/Documents/programming/c++/of/addons_diederick/roxlu/addons/Twitter/tests/https/client-cert.pem";
  const char* CLIENT_KEY_FILE = "/Users/diederickhuijbers/Documents/programming/c++/of/addons_diederick/roxlu/addons/Twitter/tests/https/client-key.pem";

  SSL_library_init();
  SSL_load_error_strings();
  RAND_poll(); // works w/o
  BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
  SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_client_method());

  int rc = SSL_CTX_use_PrivateKey_file(ssl_ctx, CLIENT_KEY_FILE, SSL_FILETYPE_PEM);
  if(!rc) {
    printf("Could not load client key file.\n");
    ::exit(1);
  }

  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, dummy_ssl_verify_callback);
  SSL_CTX_set_info_callback(ssl_ctx, dummy_ssl_info_callback); 
  SSL_CTX_set_msg_callback(ssl_ctx, dummy_ssl_msg_callback);
  SSL_CTX_set_session_cache_mode(ssl_ctx, SSL_SESS_CACHE_OFF); // testing, you can remove this


  Twitter tw;
  tw.setSSLContext(ssl_ctx);
  //#include "twitter_tokens.h"
  tw.setConsumer("q8mQUYq7AEjGX4qD8lxSKw"); // Get this from your apps settings
  tw.setConsumerSecret("Uy3toRg4OXZqwuNTk9HYTaQLX977DenO0FG8rT5v6A"); // Get this from your apps settings

  TwitterStatusesFilter tsf("twitter,love,sex,friend");
  // tw.apiStatusesFilter(tsf, on_status_update_callback); 
  tw.requestToken(on_request_token_callback, &tw);
  
  /*
    tw.apiStatusesUpdate(TwitterStatusesUpdate("__TEST123__"), on_status_update_callback);
    tw.apiStatusesUpdate(TwitterStatusesUpdate("TEST456"));
    tw.apiStatusesUpdate(TwitterStatusesUpdate("TEST786"));
  */

  while(true) {
    tw.update();
  }
  return 0;
}
