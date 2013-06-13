#ifndef ROXLU_HTTP_BUFFER_H
#define ROXLU_HTTP_BUFFER_H

#include <vector>
#include <iterator>
#include <algorithm>
#include <ssl/SSLContext.h>
#include <ssl/SSLBuffer.h>

//-------------------------------------------------------------------------------

typedef void(*httpbuffer_callback)(const char* buffer, size_t len, void* user);

//-------------------------------------------------------------------------------

class HTTPBufferBase {
 public:
  HTTPBufferBase(httpbuffer_callback cbFlushInput, httpbuffer_callback cbFlushOutput, void* user);
  virtual ~HTTPBufferBase();

  virtual void addToInputBuffer(const char* buf, size_t len) = 0;
  virtual void addToOutputBuffer(const char* buf, size_t len) = 0;  
  virtual void update() = 0;
  virtual void flushInputBuffer() = 0;
  virtual void flushOutputBuffer() = 0;
  
 public:
  size_t read_dx;
  httpbuffer_callback cb_flush_input;
  httpbuffer_callback cb_flush_output;
  void* cb_user;
};

//-------------------------------------------------------------------------------

class HTTPBuffer : public HTTPBufferBase {
 public:
  HTTPBuffer(httpbuffer_callback cbFlushInput, httpbuffer_callback cbFlushOutput, void* user);
  ~HTTPBuffer();

  void addToInputBuffer(const char* buf, size_t len);
  void addToOutputBuffer(const char* buf, size_t len);  
  void flushInputBuffer();
  void flushOutputBuffer();
  void update();

 public:
  std::vector<char> data_in;
  std::vector<char> data_out;
};

inline void HTTPBuffer::update() {
}


//-------------------------------------------------------------------------------
void httpbufferssl_on_decrypted_data(const char* data, size_t len, void* user);
void httpbufferssl_on_encrypted_data(const char* data, size_t len, void* user);

class HTTPBufferSSL : public HTTPBufferBase {
 public:
  HTTPBufferSSL(httpbuffer_callback cbFlushInput, httpbuffer_callback cbFlushOutput, void* user, SSL* ssl);
  ~HTTPBufferSSL();
  
  void addToInputBuffer(const char* buf, size_t len);
  void addToOutputBuffer(const char* buf, size_t len);  
  void flushInputBuffer();
  void flushOutputBuffer();
  void update();

 public:
  SSLBuffer data;
  std::vector<char> encrypted_data;
  std::vector<char> decrypted_data;
};
#endif
