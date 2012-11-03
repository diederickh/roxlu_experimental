#include <twitter/SSLBuffer.h>
namespace roxlu {
  namespace twitter { 

    SSLBuffer::SSLBuffer()
      :ssl(NULL)
      ,read_bio(NULL)
      ,write_bio(NULL)
      ,write_to_socket_callback(NULL)
      ,write_to_socket_callback_data(NULL)
      ,read_decrypted_callback(NULL)
      ,read_decrypted_callback_data(NULL)
    {
    }

    SSLBuffer::~SSLBuffer() {
    }

    void SSLBuffer::init(
                         SSL* s, 
                         cb_write_to_socket writeCB, 
                         void* writeData,
                         cb_read_decrypted readCB,
                         void* readData
                         ) 
    {
      ssl = s;
      read_bio = BIO_new(BIO_s_mem());
      write_bio = BIO_new(BIO_s_mem());
      SSL_set_bio(ssl, read_bio, write_bio);
      write_to_socket_callback = writeCB;
      write_to_socket_callback_data = writeData;
      read_decrypted_callback = readCB;
      read_decrypted_callback_data = readData;
    }

    void SSLBuffer::addEncryptedData(const char* data, size_t len) {
      BIO_write(read_bio, data, len);
      update();
    }

    void SSLBuffer::addApplicationData(const std::string& s) {
      addApplicationData(s.c_str(), s.size());
    }

    void SSLBuffer::addApplicationData(const char* data, size_t len) {
      std::copy(data, data+len, std::back_inserter(buffer_out));
    }

    void SSLBuffer::update() {
      if(!SSL_is_init_finished(ssl)) {
        int r = SSL_connect(ssl);
        if(r < 0) {
          handleError(r);
        }
      }
      else {
        char in_buf[1024 * 16];
        int bytes_read = 0;
        while((bytes_read = SSL_read(ssl, in_buf, sizeof(in_buf))) > 0) {
          if(read_decrypted_callback) {
            read_decrypted_callback(in_buf, bytes_read, read_decrypted_callback_data);
          }
        }      
      }
      checkOutgoingApplicationData();
    }

    // flushes encrypted data 
    void SSLBuffer::flushWriteBIO() {
      char buf[1024*16];
      int bytes_read = 0;
      while((bytes_read = BIO_read(write_bio, buf, sizeof(buf))) > 0) {
        if(write_to_socket_callback) {
          write_to_socket_callback(buf, bytes_read, write_to_socket_callback_data);
        }
      }
    }

    // Any of the SSL_* functions can cause a error. We need to handle the SSL_ERROR_WANT_READ/WRITE
    void SSLBuffer::handleError(int r) {
      int error = SSL_get_error(ssl, r);
      if(error == SSL_ERROR_WANT_READ) {
        flushWriteBIO();
      }
    }

    // Is there data pending in the out buffer which should send?
    void SSLBuffer::checkOutgoingApplicationData() {
      if(SSL_is_init_finished(ssl)) { 
        if(buffer_out.size() > 0) {
          int r = SSL_write(ssl, &buffer_out[0], buffer_out.size()); // causes the write_bio to fill up (which we need to flush)
          buffer_out.clear();
          handleError(r);
          flushWriteBIO();
        }
      }
    }

  } // roxlu
} // twitter
