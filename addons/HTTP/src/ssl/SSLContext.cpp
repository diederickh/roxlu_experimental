#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <ssl/SSLContext.h>

//-------------------------------------------------------------------------------

int sslcontext_verify(int ok, X509_STORE_CTX* store) {
  return 1;
}

void sslcontext_info(const SSL* ssl, int where, int ret) {
  if(ret == 0) {
    printf("dummy_ssl_info_callback, error occured.\n");
    return;
  }
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_EXIT, "EXIT");
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_READ, "READ");
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_WRITE, "WRITE");
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_ALERT, "ALERT");
  SSLCONTEXT_WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
}

//-------------------------------------------------------------------------------

SSLContext::SSLContext()
  :ssl_ctx(NULL)
{
  RAND_poll();
  SSL_library_init();
  SSL_load_error_strings();

  ssl_ctx = SSL_CTX_new(SSLv23_client_method());
  if(!ssl_ctx) {
    RX_ERROR("Cannot create a new SSL context");
    ::exit(EXIT_FAILURE);
  }

  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, sslcontext_verify);
  SSL_CTX_set_info_callback(ssl_ctx, sslcontext_info);
}

SSLContext::~SSLContext() {
  if(ssl_ctx) {
    SSL_CTX_free(ssl_ctx);
  }
}
