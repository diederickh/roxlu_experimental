#include <iostream>
#include <libgen.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <uv.h>
#include <vector>
#include <iterator>
#include <algorithm>

#define EXIT(msg) { printf(msg); ::exit(0); }

#define WHERE_INFO(ssl, w, flag, msg) { \
    if(w & flag) { \
      printf("\t"); \
      printf(msg); \
      printf(" - %s ", SSL_state_string(ssl)); \
      printf(" - %s ", SSL_state_string_long(ssl)); \
      printf("\n"); \
    }\
 } 

// INFO CALLBACK
void dummy_ssl_info_callback(const SSL* ssl, int where, int ret) {
  if(ret == 0) {
    printf("dummy_ssl_info_callback, error occured.\n");
    return;
  }
  WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
  WHERE_INFO(ssl, where, SSL_CB_EXIT, "EXIT");
  WHERE_INFO(ssl, where, SSL_CB_READ, "READ");
  WHERE_INFO(ssl, where, SSL_CB_WRITE, "WRITE");
  WHERE_INFO(ssl, where, SSL_CB_ALERT, "ALERT");
  WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
}

// MSG CALLBACK
void dummy_ssl_msg_callback(
                            int writep
                            ,int version
                            ,int contentType
                            ,const void* buf
                            ,size_t len
                            ,SSL* ssl
                            ,void *arg
                            ) 
{
  printf("\tMessage callback with length: %zu\n", len);
}

// VERIFY
int dummy_ssl_verify_callback(int ok, X509_STORE_CTX* store) {
  char buf[256];
  X509* err_cert;
  err_cert = X509_STORE_CTX_get_current_cert(store);
  int err = X509_STORE_CTX_get_error(store);
  int depth = X509_STORE_CTX_get_error_depth(store);
  X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);

  BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
  X509_NAME* cert_name = X509_get_subject_name(err_cert);
  X509_NAME_print_ex(outbio, cert_name, 0, XN_FLAG_MULTILINE);
  BIO_free_all(outbio);
  printf("\tssl_verify_callback(), ok: %d, error: %d, depth: %d, name: %s\n", ok, err, depth, buf);

  return 1;  // We always return 1, so no verification actually
}

// LIBUV
// ------
void on_written_callback(uv_write_t* req, int status);

struct Client {
  void addAppData(const std::string str) {
    std::copy(str.begin(), str.end(), std::back_inserter(buffer_out));
  }

  uv_loop_t* loop;
  uv_tcp_t socket;
  uv_write_t write_req;
  uv_connect_t connect_req;
  char host[1024];
  char port[1024];
  char page[1024];
  std::vector<char> buffer_in; // app data in
  std::vector<char> buffer_out; // app data out
  SSL_CTX* ssl_ctx;
  SSL* ssl;
  BIO* read_bio;
  BIO* write_bio;
};

void write_to_socket(Client* c, char* buf, size_t len) {
  if(len <= 0) {
    return;
  }
  uv_buf_t uvbuf;
  uvbuf.base = buf;
  uvbuf.len = len;
  int r = uv_write(&c->write_req, (uv_stream_t*)&c->socket, &uvbuf, 1, on_written_callback);
  if(r < 0) {
    printf("ERROR: write_to_socket error: %s\n", uv_err_name(uv_last_error(c->socket.loop)));
  }
}

void flush_read_bio(Client* c) {
  char buf[1024*16];
  int bytes_read = 0;
  while((bytes_read = BIO_read(c->write_bio, buf, sizeof(buf))) > 0) {
    write_to_socket(c, buf, bytes_read);
  }
}

void handle_error(Client* c, int result) {
  int error = SSL_get_error(c->ssl, result);
  if(error == SSL_ERROR_WANT_READ) { // wants to read from bio
    flush_read_bio(c);
  }
}

void check_outgoing_application_data(Client* c) {    
  if(SSL_is_init_finished(c->ssl)) {
    if(c->buffer_out.size() > 0) {
      std::copy(c->buffer_out.begin(), c->buffer_out.end(), std::ostream_iterator<char>(std::cout,""));
      int r = SSL_write(c->ssl, &c->buffer_out[0], c->buffer_out.size());
      c->buffer_out.clear();
      handle_error(c, r);
      flush_read_bio(c);
    }
  }
}

// HANDLE BUFFERS HERE!
// --------------------
void on_event(Client* c) { // is called after each socket event
  char buf[1024 * 10];
  int bytes_read = 0;

  if(!SSL_is_init_finished(c->ssl)) {
    int r = SSL_connect(c->ssl);
    if(r < 0) {
      handle_error(c, r);
    }
    check_outgoing_application_data(c);
  }
  else {
    // connect, check if there is encrypted data, or we need to send app data
    int r = SSL_read(c->ssl, buf, sizeof(buf));
    if(r < 0) {
      handle_error(c, r);
    }
    else if(r > 0) {
      std::copy(buf, buf+r, std::back_inserter(c->buffer_in));
      std::copy(c->buffer_in.begin(), c->buffer_in.end(), std::ostream_iterator<char>(std::cout));
      c->buffer_in.clear();
    }
    check_outgoing_application_data(c);
  }
}

uv_buf_t on_alloc_callback(uv_handle_t* con, size_t size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(size);
  buf.len = size;
  return buf;
}

void on_written_callback(uv_write_t* req, int status) { }

void on_read_callback(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) {
  Client* c = static_cast<Client*>(tcp->data);
  if(nread == -1) { // disconnected (?)
    char plain_buf[1024*10];
    int r = SSL_read(c->ssl, plain_buf, sizeof(plain_buf));
    if(r < 0) {
      handle_error(c, r);
    }
    else if(r > 0) {
      std::copy(plain_buf, plain_buf+r, std::back_inserter(c->buffer_in));
    }
    std::copy(c->buffer_in.begin(), c->buffer_in.end(), std::ostream_iterator<char>(std::cout));
    ::exit(0);
  }
  int written = BIO_write(c->read_bio, buf.base, nread);     
  on_event(c);
}

void on_connect_callback(uv_connect_t* con, int status) {
  Client* c = static_cast<Client*>(con->data);
  if(status == -1) {
    printf("ERROR: on_connect_callback %s\n", uv_err_name(uv_last_error(c->loop)));
    ::exit(0);
  }

  int r = uv_read_start((uv_stream_t*)&c->socket, on_alloc_callback, on_read_callback);
  if(r == -1) {
    printf("ERROR: uv_read_start error: %s\n", uv_err_name(uv_last_error(c->loop)));
    ::exit(0);
  }

  const char* http_request_tpl = "" \
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: uv_www_client/0.1\r\n" 
    "Accept: */*\r\n"
    "Connection: close\r\n"
    "\r\n";

  char http_request[1024];
  sprintf(http_request, http_request_tpl, c->page, c->host);
  c->addAppData(http_request);
  printf("APP DATA: %zu\n", c->buffer_out.size());
  c->ssl = SSL_new(c->ssl_ctx);
  c->read_bio = BIO_new(BIO_s_mem());
  c->write_bio = BIO_new(BIO_s_mem());
  SSL_set_bio(c->ssl, c->read_bio, c->write_bio);
  SSL_set_connect_state(c->ssl);

  r = SSL_do_handshake(c->ssl);

  on_event(c);
}

void on_resolved_callback(uv_getaddrinfo_t* resolver, int status, struct addrinfo * res) {
  Client* c = static_cast<Client*>(resolver->data);
  if(status == -1) {
    printf("ERROR: getaddrinfo callback error: %s\n", uv_err_name(uv_last_error(c->loop)));
    ::exit(0);
  }
  
  char addr[17] = {'\0'};
  uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 16);
  printf("Found host:  %s\n", addr);

  uv_tcp_init(c->loop, &c->socket);
  uv_tcp_connect(&c->connect_req, &c->socket, *(sockaddr_in*)res->ai_addr, on_connect_callback);
  uv_freeaddrinfo(res);
}

// --------------------
int main() {
  char client_key_file[1024];
  sprintf(client_key_file, "%s/%s", dirname(__FILE__), "client-key.pem");
 
  // Initialize SSL
  SSL_library_init();
  SSL_load_error_strings();

  BIO* bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
  SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_client_method());
  int rc = SSL_CTX_use_PrivateKey_file(ssl_ctx, client_key_file, SSL_FILETYPE_PEM);
  if(!rc) {
    EXIT("Could not load client key file.\n");
  }

  SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2); 
  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, dummy_ssl_verify_callback); // our callback always returns true, so no validation
  SSL_CTX_set_info_callback(ssl_ctx, dummy_ssl_info_callback);  // for dibugging
  SSL_CTX_set_msg_callback(ssl_ctx, dummy_ssl_msg_callback);

  uv_loop_t* loop = uv_loop_new();

  // Client context
  Client c;
  c.loop = loop;
  c.connect_req.data = &c;
  c.socket.data = &c;
  c.ssl = NULL;
  c.ssl_ctx = ssl_ctx;

  sprintf(c.host, "%s", "test.localhost");
  sprintf(c.port, "%s", "443");
  sprintf(c.page, "%s", "/chunked.php");

  // Resolve host
  struct addrinfo hints;
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
  uv_getaddrinfo_t resolver;
  resolver.data = &c;
  int r = uv_getaddrinfo(loop, &resolver, on_resolved_callback, c.host, c.port, &hints);
  uv_run(loop);
  return 0;
}


