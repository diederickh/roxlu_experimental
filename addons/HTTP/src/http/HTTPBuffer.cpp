#include <roxlu/core/Log.h>
#include <algorithm>
#include <http/HTTPBuffer.h>

HTTPBufferBase::HTTPBufferBase(httpbuffer_callback cbFlushInput, 
                               httpbuffer_callback cbWrite, 
                               void* user)
  :read_dx(0)
  ,cb_flush_input(cbFlushInput)
  ,cb_flush_output(cbWrite)
  ,cb_user(user)
{
}

HTTPBufferBase::~HTTPBufferBase() {
}


//-------------------------------------------------------------------------------

HTTPBuffer::HTTPBuffer(httpbuffer_callback cbFlushInput,
                       httpbuffer_callback cbWrite, 
                       void* user)
  :HTTPBufferBase(cbFlushInput, cbWrite, user)
{
}

HTTPBuffer::~HTTPBuffer() {
}

void HTTPBuffer::addToInputBuffer(const char* buf, size_t len) {
  std::copy(buf, buf+len, std::back_inserter(data_in));
  flushInputBuffer();
}

void HTTPBuffer::addToOutputBuffer(const char* buf, size_t len) {
  std::copy(buf, buf+len, std::back_inserter(data_out));
}

void HTTPBuffer::flushInputBuffer() {
  if(cb_flush_input) {
    cb_flush_input(&data_in[0], data_in.size(), cb_user);
    data_in.clear();
  }
}

void HTTPBuffer::flushOutputBuffer() {
  if(cb_flush_output) {
    cb_flush_output(&data_out[0], data_out.size(), cb_user);
    data_out.clear();
  }
}

//-------------------------------------------------------------------------------

void httpbufferssl_on_decrypted_data(const char* data, size_t len, void* user) {
  HTTPBufferSSL* buf = static_cast<HTTPBufferSSL*>(user);
  std::copy(data, data+len, std::back_inserter(buf->decrypted_data));
  buf->flushInputBuffer();
}

void httpbufferssl_on_encrypted_data(const char* data, size_t len, void* user) {
  HTTPBufferSSL* buf = static_cast<HTTPBufferSSL*>(user);
  std::copy(data, data + len, std::back_inserter(buf->encrypted_data));
  buf->flushOutputBuffer();
}

HTTPBufferSSL::HTTPBufferSSL(httpbuffer_callback cbFlushInput, 
                             httpbuffer_callback cbWrite, 
                             void* user, 
                             SSL* ssl) 
  :HTTPBufferBase(cbFlushInput, cbWrite, user)
{
  bool r = data.setup(ssl, httpbufferssl_on_decrypted_data, httpbufferssl_on_encrypted_data, this);
  if(!r) {
    RX_ERROR("Error while setting up the SSL buffer");
  }
}

HTTPBufferSSL::~HTTPBufferSSL() {
}
  
void HTTPBufferSSL::addToInputBuffer(const char* buf, size_t len) {
  data.addEncryptedData(buf, len);
  data.update();
}

void HTTPBufferSSL::update() {
  data.update();
}

void HTTPBufferSSL::addToOutputBuffer(const char* buf, size_t len) {
  data.addRawData(buf, len);
}  


void HTTPBufferSSL::flushInputBuffer() {
  if(!cb_flush_input) {
    RX_ERROR("No input flush callback set");
    return;
  }
  printf("-------------------------------------------\n");
  for(size_t i = 0; i < decrypted_data.size(); ++i) {
    printf("%c", decrypted_data[i]);
  }
  printf("=============================================\n");
  cb_flush_input(&decrypted_data[0], decrypted_data.size(), cb_user);
  decrypted_data.clear();
}

void HTTPBufferSSL::flushOutputBuffer() {
  if(!cb_flush_output) {
    RX_ERROR("No sent IO handler set!");
  }
  if(!encrypted_data.size()) {
    return;
  }
  cb_flush_output(&encrypted_data[0], encrypted_data.size(), cb_user);
  encrypted_data.clear();
}

