#ifndef ROXLU_OPENSSL_BUFFERH
#define ROXLU_OPENSSL_BUFFERH

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <string>
#include <vector>

/** 
  Wrapper for memory bios + non-blocking / async sockets

  Usage, based on libuv (libevent or libev should be something similar):

  1) resolve a host, connect with tcp
  2) once connected:
       - create a SSL*
       - call: ssl_buffer.init(...)
       - call: SSL_set_connect_state(ssl)
       - call: SSL_do_handshake()
       - call: ssl_buffer.update()
  3) when you receive data from the socket, add this to the buffer:
       - call: ssl_buffer.addEncryptedData()
       - call: ssl_buffer.update()


  - Whenever you need to write to the socket the 'write to socket callback' is called 
  - Whenever there is decrypted data available the 'read decrypted data callback' is called

  - For an example see: https://gist.github.com/gists/4000573/edit
*/


typedef void (*cb_write_to_socket)(const char* data, size_t len, void* userdata);
typedef void (*cb_read_decrypted)(const char* data, size_t len, void* userdata);

// SSL CLIENT BUFFER
class SSLBuffer {
public:
  SSLBuffer();
  ~SSLBuffer();
  void init(
            SSL* ssl,
            cb_write_to_socket writeCB, 
            void* writeData,
            cb_read_decrypted readCB,
            void* readData
            );
  void update();
  void addEncryptedData(const char* data, size_t len);
  void addApplicationData(const std::string& data);
  void addApplicationData(const char* data, size_t len);
private:
  void handleError(int r);
  void checkOutgoingApplicationData();
  void flushWriteBIO();
private:
  cb_write_to_socket write_to_socket_callback;
  cb_read_decrypted read_decrypted_callback;
  void* write_to_socket_callback_data;
  void* read_decrypted_callback_data;
  SSL* ssl;
  BIO* read_bio; // SSL reads from this buffer (so we write encrypted data into this)
  BIO* write_bio; // SSL writes into this buffer (so we need to send this to the server)
  std::vector<char> buffer_out; // application data, what needs to be encrypted and send to server
};

#endif
