#ifndef INSTAGRAM_SSLBUFFERH
#define INSTAGRAM_SSLBUFFERH

//extern "C" {
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
//};

#include <string>
#include <vector>

typedef void(*instagram_cb_write_to_socket)(const char* data, size_t len, void* userdata);
typedef void(*instagram_cb_read_decrypted)(const char* data, size_t len, void* userdata);

    
// http://stackoverflow.com/questions/4403816/io-completion-ports-and-openssl

namespace roxlu { 
  class SSLBuffer {
  public:
    SSLBuffer();
    ~SSLBuffer();
    void init(
              SSL* ssl,
              instagram_cb_write_to_socket writeCB, 
              void* writeData, 
              instagram_cb_read_decrypted readCB,
              void* readData
              );
    void addApplicationData(const char* data, size_t len);
    void addEncryptedData(const char* data, size_t len);
    void update();
  private:
    void handleError(int err);
    void flushWriteBIO();
    void checkApplicationData();
  public:
    SSL* ssl;
    BIO* read_bio;
    BIO* write_bio;
    instagram_cb_write_to_socket write_cb;
    void* write_data;
    instagram_cb_read_decrypted read_cb;
    void* read_data;
    std::vector<char> out_buf;
    char in_buf[1024 * 16];

  };

} // roxlu
#endif
