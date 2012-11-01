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
  printf("\t<Message callback with length: %zu>\n", len);
}

int dummy_ssl_verify_callback(int ok, X509_STORE_CTX* store) {
  char buf[256];
  X509* err_cert;
  err_cert = X509_STORE_CTX_get_current_cert(store);
  int err = X509_STORE_CTX_get_error(store);
  int depth = X509_STORE_CTX_get_error_depth(store);
  X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);
  printf("\tssl_verify_callback(), ok: %d, error: %d, depth: %d, name: %s\n", ok, err, depth, buf);
  return 1;
}

int main() {
  const char* CLIENT_CERT_FILE = "/Users/diederickhuijbers/Documents/programming/c++/of/addons_diederick/roxlu/addons/Twitter/tests/https/client-cert.pem";
  const char* CLIENT_KEY_FILE = "/Users/diederickhuijbers/Documents/programming/c++/of/addons_diederick/roxlu/addons/Twitter/tests/https/client-key.pem";

  SSL_library_init();
  SSL_load_error_strings();
  RAND_poll(); // works w/o
  BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
  //  SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_method());
  SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_client_method());

  int rc = SSL_CTX_use_certificate_chain_file(ssl_ctx, CLIENT_CERT_FILE);
  if(rc != 1) {
    printf("Could not load client certificate file.\n");
    ::exit(1);
  }

  rc = SSL_CTX_use_PrivateKey_file(ssl_ctx, CLIENT_KEY_FILE, SSL_FILETYPE_PEM);
  if(!rc) {
    printf("Could not load client key file.\n");
    ::exit(1);
  }

  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
  //  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, dummy_ssl_verify_callback);
  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL); // no verify, testing libuv + openssl mem bios
  SSL_CTX_set_info_callback(ssl_ctx, dummy_ssl_info_callback); 
  SSL_CTX_set_msg_callback(ssl_ctx, dummy_ssl_msg_callback);
  SSL_CTX_set_session_cache_mode(ssl_ctx, SSL_SESS_CACHE_OFF); // testing, you can remove this

  SSL_CTX_set_cipher_list(ssl_ctx, "RC4+RSA:HIGH:+MEDIUM:+LOW"); // testing, you can remove this

  Twitter tw;
  tw.setSSLContext(ssl_ctx);
#include "twitter_tokens.h"

  //  TwitterStatusesFilter tsf("twitter,love,sex,friend");
  //tw.apiStatusesFilter(tsf);
  tw.apiStatusesUpdate(TwitterStatusesUpdate("TEST123"));

  while(true) {
    tw.update();
  }
  return 0;
}


/*
Async HTTP client + ssl: http://code.google.com/p/craplib/source/browse/trunk/HTTPRequest.cpp
MailingList, big post on memory bios + async + debug tools http://www.mail-archive.com/openssl-users@openssl.org/msg51773.html
SSL, handshake + memory BIOs flow: http://www.mail-archive.com/openssl-users@openssl.org/msg39512.html
High level description on mem bio: http://stackoverflow.com/questions/4403816/io-completion-ports-and-openssl
SSL 3.0 RFC: http://tools.ietf.org/html/draft-ietf-tls-ssl-version3-00
Nice clean tutorial + flow example: http://www.ee.up.ac.za/main/_media/en/undergrad/subjects/ehn410/ehn410_prac1_guide_2012.pdf
Using openSSL nice clean with code: http://blog.davidwolinsky.com/2009/10/memory-bios-and-openssl.html
Using openSSL in applications: http://zzz.zggg.com/2005/05/05/how-to-programmatically-create-self-signed-cert-key-pair-windows-sspi/
Creating client certificates: http://drumcoder.co.uk/blog/2011/oct/19/client-side-certificates-web-apps/
LibEvent + SSLL http://www.wangafu.net/~nickm/libevent-book/Ref6a_advanced_bufferevents.html
MySQL info, used in example above: http://dev.mysql.com/doc/refman/5.1/en/creating-ssl-certs.html
Client SSL Certs: http://itpro.comptia.org/security_1/b/weblog1/archive/2012/01/06/creating-certificate-authorities-and-client-certificates.aspx
Exmample SSL + libevent https: http://www.provos.org/index.php?/archives/79-OpenSSL-Client-Certificates-and-Libevent-2.0.3-alpha.html
Apache SSL flow: http://httpd.apache.org/docs/2.2/ssl/ssl_intro.html
Certificates for SSL Applications: http://h71000.www7.hp.com/doc/83final/ba554_90007/ch04s02.html

 */
