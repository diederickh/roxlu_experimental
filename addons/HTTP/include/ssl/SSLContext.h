#ifndef ROXLU_SSLCONTEXT_H
#define ROXLU_SSLCONTEXT_H

extern "C" {
#  include <openssl/ssl.h>
#  include <openssl/bio.h>
#  include <openssl/err.h>  
#  include <openssl/rand.h>  
}

#include <string>

#define SSLCONTEXT_WHERE_INFO(ssl, w, flag, msg) {  \
    if(w & flag) {                                  \
      printf("\t");                                 \
      printf(msg);                                  \
      printf(" - %s ", SSL_state_string(ssl));      \
      printf(" - %s ", SSL_state_string_long(ssl)); \
      printf("\n");                                 \
    }                                               \
  } 

//-------------------------------------------------------------------------------

int sslcontext_verify(int ok, X509_STORE_CTX* store);
void sslcontext_info(const SSL* ssl, int where, int ret);

//-------------------------------------------------------------------------------

class SSLContext {
 public:
  SSLContext();
  ~SSLContext();
  SSL* allocateSSL();                                                    /* caller is responsible for managing memory! */

 public:
  SSL_CTX* ssl_ctx;
};

inline SSL* SSLContext::allocateSSL() {
  return SSL_new(ssl_ctx);
}

#endif
