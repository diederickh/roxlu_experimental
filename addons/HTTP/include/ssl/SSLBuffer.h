#ifndef ROXLU_SSLBUFFER_H
#define ROXLU_SSLBUFFER_H

extern "C" {
#  include <openssl/ssl.h>
#  include <openssl/bio.h>
#  include <openssl/err.h>  
}

#include <vector>
#include <iterator>
#include <algorithm>

typedef void(*sslbuffer_callback)(const char* data, size_t len, void* user);

class SSLBuffer {
public:
  SSLBuffer();
  ~SSLBuffer();
  bool setup(SSL* ssl, sslbuffer_callback onDecryptedData, sslbuffer_callback onEncryptedData, void* user);
  void addRawData(const char* buf, size_t len);
  void addEncryptedData(const char* buf, size_t len);
  void update();

 private:
  void flushDestBIO();
  void flushSourceBIO();
  void handleError(int r);

 public:
  SSL* ssl;
  BIO* source_bio;                     /* SSL reads from this bio (so we write encrypted data into this) */
  BIO* dest_bio;                       /* SSL writes into this bio (so we need to read from this // use this to sent to another server) */
  std::vector<char> raw_data;          /* data that needs to be encoded (application data / clear text for example) */
  sslbuffer_callback cb_on_decrypted;  /* gets called when we have decrypted some data, passing the decrypted data to the callback */
  sslbuffer_callback cb_on_encrypted;  /* gets called when we have encrypted data, passing the data to the callback */
  void* cb_user;
};

#endif
