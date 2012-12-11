#include <instagram/SSLBuffer.h>
namespace roxlu {

  SSLBuffer::SSLBuffer() 
    :ssl(NULL)
    ,write_cb(NULL)
    ,write_data(NULL)
    ,read_cb(NULL)
    ,read_data(NULL)
    ,read_bio(NULL)
    ,write_bio(NULL)
  {
  }

  SSLBuffer::~SSLBuffer() {
  }

  void SSLBuffer::init(
                       SSL* s,
                       instagram_cb_write_to_socket writeCB, 
                       void* writeData, 
                       instagram_cb_read_decrypted readCB,
                       void* readData
                       )
  {
    ssl = s;
    read_bio = BIO_new(BIO_s_mem());
    if(read_bio == NULL) {
      printf("ERROR: cannot create read bio.\n");
      return;
    }

    write_bio = BIO_new(BIO_s_mem());
    if(write_bio == NULL) {
      printf("ERROR: cannot create write bio.\n");
      return;
    }

    SSL_set_bio(ssl, read_bio, write_bio);
    write_cb = writeCB;
    write_data = writeData;
    read_cb = readCB;
    read_data = readData;
  }

  void SSLBuffer::addApplicationData(const char* data, size_t len) {
    std::copy(data, data+len, std::back_inserter(out_buf));
  }

  void SSLBuffer::addEncryptedData(const char* data, size_t len) {
    BIO_write(read_bio, data, len);
    update();
  }

  void SSLBuffer::update() {
    if(!SSL_is_init_finished(ssl)) {
      int r = SSL_connect(ssl);
      if(r < 0) {
        handleError(r);
      }
    }
    else {
      int bytes_read = 0;
      while((bytes_read = SSL_read(ssl, in_buf, sizeof(in_buf))) > 0) {
        if(read_cb) {
          read_cb(in_buf, bytes_read, read_data);
        }
      }
    }
    checkApplicationData();
  }

  void SSLBuffer::checkApplicationData() {
    if(SSL_is_init_finished(ssl)) {
      if(out_buf.size() > 0) {
        int r = SSL_write(ssl, &out_buf[0], out_buf.size());
        out_buf.clear();
        handleError(r);
        flushWriteBIO();
      }
    }
  }

  void SSLBuffer::handleError(int err) {
    int error = SSL_get_error(ssl, err);
    if(error == SSL_ERROR_WANT_READ) {
      flushWriteBIO();
    }
    else if(error == SSL_ERROR_SSL) {
      // log error
      char buf[256];
      u_long err = error;
      while ((err = ERR_get_error()) != 0) {
        ERR_error_string_n(err, buf, sizeof(buf));
        printf("*** %s\n", buf);
      }
    }
    flushWriteBIO();
  }

  void SSLBuffer::flushWriteBIO() {
    char tmp_buf[1024*16];
    int bytes_read = 0;
    
    while((bytes_read = BIO_read(write_bio, tmp_buf, sizeof(tmp_buf)))> 0) {
      if(write_cb) {
        write_cb(tmp_buf, bytes_read, write_data);
      }
    }
  }
} // roxlu

