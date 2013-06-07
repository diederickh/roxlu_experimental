#include <roxlu/core/Log.h>
#include <ssl/SSLBuffer.h>

SSLBuffer::SSLBuffer() 
  :ssl(NULL)
  ,source_bio(NULL)
  ,dest_bio(NULL)
  ,cb_on_decrypted(NULL)
  ,cb_on_encrypted(NULL)
  ,cb_user(NULL)
{
  source_bio = BIO_new(BIO_s_mem());
  dest_bio = BIO_new(BIO_s_mem());
}

SSLBuffer::~SSLBuffer() {
}

// Setup the buffer with the necessary callbacks for I/O
bool SSLBuffer::setup(SSL* s, sslbuffer_callback onDecryptedData, sslbuffer_callback onEncryptedData, void* user) {
  if(!source_bio || !dest_bio) {
    RX_VERBOSE("The source and dest BIO are invalid. Out of memory?");
    return false;
  }

  ssl = s;
  SSL_set_bio(ssl, source_bio, dest_bio);
  cb_on_decrypted = onDecryptedData;
  cb_on_encrypted = onEncryptedData;
  cb_user = user;

  return true;
}

// Add clear/decrypted data to the buffer
void SSLBuffer::addRawData(const char* buf, size_t len) {
  std::copy(buf, buf+len, std::back_inserter(raw_data));
}

// Add encrypted data to the buffer
void SSLBuffer::addEncryptedData(const char* buf, size_t len) {
  int r = BIO_write(source_bio, buf, len);
  if(r < 0) {
    RX_VERBOSE("Unhandled SSL addEncryptedData error");
  }
  update();
}

// Updates the SSL bio states and en/decrypts
void SSLBuffer::update() {
  if(!SSL_is_init_finished(ssl)) {
    int r = SSL_connect(ssl);
    if(r < 0) {
      handleError(r);
    }
  }
  else {
    char in_buf[1024*16];
    int bytes_read = 0;
    while((bytes_read = SSL_read(ssl, in_buf, sizeof(in_buf))) > 0) {
      if(cb_on_decrypted) {
        cb_on_decrypted(in_buf, bytes_read, cb_user);
      }
    }      
  }
  flushSourceBIO();
}

// Flushes encrypted data
void SSLBuffer::flushDestBIO() {
  char buf[1024*16];
  int bytes_read = 0;
  while((bytes_read = BIO_read(dest_bio, buf, sizeof(buf))) > 0) {
    if(cb_on_encrypted) {
      cb_on_encrypted(buf, bytes_read, cb_user);
    }
  }
}

// Is there data pending in the out buffer which should send?
void SSLBuffer::flushSourceBIO() {
  if(SSL_is_init_finished(ssl)) { 
    if(raw_data.size() > 0) {
      int r = SSL_write(ssl, &raw_data[0], raw_data.size()); // causes the source_bio to fill up (which we need to flush)
      raw_data.clear();
      handleError(r);
      flushDestBIO();
    }
  }
}

// Any of the SSL_* functions can cause a error. We need to handle the SSL_ERROR_WANT_READ/WRITE
void SSLBuffer::handleError(int r) {
  int error = SSL_get_error(ssl, r);
  if(error == SSL_ERROR_WANT_READ) {
    flushDestBIO();
  }
}
