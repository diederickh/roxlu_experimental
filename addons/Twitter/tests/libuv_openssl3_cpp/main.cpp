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
#include <twitter/SSLBuffer.h>

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
  return 1;  // We always return 1, so no verification actually
}

// LIBUV
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
  SSLBuffer ssl_buffer;
  SSL* ssl;
};

void write_to_socket_callback(const char* data, size_t len, void* client) {
  if(len <= 0) {
    return;
  }
  Client* c = static_cast<Client*>(client);
  uv_buf_t uvbuf;
  uvbuf.base = (char*)data;
  uvbuf.len = len;
  int r = uv_write(&c->write_req, (uv_stream_t*)&c->socket, &uvbuf, 1, NULL);
  if(r < 0) {
    printf("ERROR: write_to_socket error: %s\n", uv_err_name(uv_last_error(c->socket.loop)));
  }
}

void read_decrypted_callback(const char* data, size_t len, void* client) {
  std::copy(data, data+len, std::ostream_iterator<char>(std::cout, ""));
}

// HANDLE BUFFERS HERE!
uv_buf_t on_alloc_callback(uv_handle_t* con, size_t size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(size);
  buf.len = size;
  return buf;
}

void on_read_callback(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) {
  Client* c = static_cast<Client*>(tcp->data);
  c->ssl_buffer.addEncryptedData(buf.base, nread);
  c->ssl_buffer.update();
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
  c->ssl_buffer.addApplicationData(http_request);
  c->ssl = SSL_new(c->ssl_ctx);

  SSL_set_connect_state(c->ssl);
  SSL_do_handshake(c->ssl);

  c->ssl_buffer.init(c->ssl, write_to_socket_callback, c, read_decrypted_callback, c);
  c->ssl_buffer.update();
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


