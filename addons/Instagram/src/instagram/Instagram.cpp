#include <instagram/Instagram.h>

namespace roxlu {

  // ---------------------------------------------------------
  // SSL Callbacks
  // ---------------------------------------------------------
  // INFO CALLBACK

#define WHERE_INFO(ssl, w, flag, msg) {                 \
    if(w & flag) {                                      \
      printf("\t");                                     \
      printf(msg);                                      \
      printf(" - %s ", SSL_state_string(ssl));          \
      printf(" - %s ", SSL_state_string_long(ssl));     \
      printf("\n");                                     \
    }                                                   \
  } 
  void dummy_ssl_info_callback(const SSL* ssl, int where, int ret) {
    if(ret == 0) {
      printf("dummy_ssl_info_callback, error occured.\n");
      char buf[256];
      u_long err;

      while ((err = ERR_get_error()) != 0) {
        ERR_error_string_n(err, buf, sizeof(buf));
        printf("*** %s\n", buf);
      }

      //      return;
    }
    WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
    WHERE_INFO(ssl, where, SSL_CB_EXIT, "EXIT");
    WHERE_INFO(ssl, where, SSL_CB_READ, "READ");
    WHERE_INFO(ssl, where, SSL_CB_WRITE, "WRITE");
    WHERE_INFO(ssl, where, SSL_CB_ALERT, "ALERT");
    WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
  }

  int instagram_ssl_verify_peer(int ok, X509_STORE_CTX* store) {
    return 1;
  }

  // ---------------------------------------------------------
  // Libuv Callbacks
  // ---------------------------------------------------------
  uv_buf_t instagram_alloc_buffer(uv_handle_t* buffer, size_t suggestedSize) {
    return uv_buf_init((char*)malloc(suggestedSize), suggestedSize);
  }

  void instagram_idle_watch(uv_idle_t* handle, int status) {
    Instagram* ins = static_cast<Instagram*>(handle->data);
    printf("CURLS: %zu\n", ins->curls.size());
    if(ins->must_stop_idler) {
      printf("STOP IDLER!!!\n");
      uv_idle_stop(handle);
    }
  }
  
  void instagram_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++ READ\n");
    InstagramConnection* con = static_cast<InstagramConnection*>(stream->data);

    if(nread < 0) {
      if(buf.base) {
        free(buf.base);
      }
      con->state = ICS_NONE;

      return;
    }
    else if(nread == 0) {
      free(buf.base);
      return;
    }
    else {
      std::copy(buf.base, buf.base+nread, std::back_inserter(con->buffer));
      
      // Handle the subscription request.
      if(con->state == ICS_PARSE_HUB_CHALLENGE) {
        std::string line;
        std::stringstream ss(con->buffer);

        while(std::getline(ss, line)) {
          size_t pos = line.find("GET /");
          if(pos != std::string::npos) {
            struct slre reg;
            struct cap capt[4 + 1];
            if(!slre_compile(&reg, "GET /callback.html\\?hub.challenge=(\\S+)&hub.mode")) { // we need to escape the ?
              printf("ERROR: cannot compile regexp\n");
              return;
            }
            if(slre_match(&reg, line.c_str(), line.size(), capt)) {
              char* res = new char[capt[1].len];
              memcpy(res, line.c_str()+33, capt[1].len);
              std::stringstream http;
              http << "HTTP/1.1 200 OK\r\n"
                   << "Content-Type: text/html\r\n"
                   << "Content-Length: " << capt[1].len << "\r\n"
                   << "\r\n"
                   << res;
              
              printf("\n+++++++++++++++++++++++++++++++++++++\n\n");
              printf("'%s'", http.str().c_str());
              printf("\n+++++++++++++++++++++++++++++++++++++\n\n");
              con->send(http.str().c_str(), http.str().size());
              delete[] res;
            }

            /*
    struct slre reg;
    struct cap capt[4 + 1];
    if(!slre_compile(&reg, "GET /callback.html\\?hub.challenge=(\\S+)&hub.mode")) {
      printf("ERROR: cannot compile regexp\n");
      return 0;
    }
    if(slre_match(&reg, line.c_str(), line.size(), capt)) {
      printf("Capt: %d, %s, %d\n", capt[1].len, capt[1].ptr, capt[0].len);
      char* res = new char[capt[1].len];
      memcpy(res, line.c_str()+33, capt[1].len);
      printf(">> %s\n", res);
    }
            */


            // Parse the token
            /*
            char token[512];
            pcre* re = NULL;
            const char* errmsg;
            int err;
            int offsets[5];
            re = pcre_compile("(.*)hub.challenge=(.*)&(.*)", PCRE_UNGREEDY | PCRE_DOTALL | PCRE_EXTENDED, &errmsg, &err, NULL);
            pcre_exec(re, NULL, line.c_str(), line.size(), 0, 0, offsets, 3);
            if(offsets[0] != -1 && offsets[1] > 0) {
              int nchars = offsets[1] - 21;
              if(nchars > 0 && nchars < 512) {
                memcpy(token, line.c_str()+20, nchars);
                token[nchars] = '\0';
                std::stringstream http;
                http << "HTTP/1.1 200 OK\r\n"
                     << "Connection: close\r\n"
                     << "\r\n"
                     << token;
                printf("Found token: %s\n", token);
                printf("\n+++++++++++++++++++++++++++++++++++++\n\n");
                printf("%s", http.str().c_str());
                printf("\n+++++++++++++++++++++++++++++++++++++\n\n");
                con->send(http.str().c_str(), http.str().size());
              }
            */
            }
          }
        }
        printf("%s\n", con->buffer.c_str());
      }
  }

  void instagram_after_write(uv_write_t* req, int status) {
    printf(">>> AFTER WRITE, STATUS: %d\n", status);
  }

  void instagram_on_new_connection(uv_stream_t* server, int status) {
    printf("VERBOSE: on new connection.\n");
    if(status == -1) {
      printf("ERROR: cannot accept new connection.\n");
      return;
    }

    Instagram* instagram = static_cast<Instagram*>(server->data);
    InstagramConnection* con = new InstagramConnection(instagram);
    con->client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    con->client->data = con;
    con->state = ICS_PARSE_HUB_CHALLENGE;
    con->write_req.data = con;
    instagram->clients.push_back(con);
    int r = uv_tcp_init(instagram->loop, con->client);
    assert(r == 0);

    if(uv_accept(server, (uv_stream_t*)con->client) == 0) {
      uv_read_start((uv_stream_t*)con->client, instagram_alloc_buffer, instagram_on_read);
    }
  }

  // ---------------------------------------------------------
  // INSTRAGRAM CONNECTION
  // ---------------------------------------------------------
  InstagramConnection::InstagramConnection(Instagram* instagram)
    :instagram(instagram)
    ,client(NULL)
    ,state(ICS_PARSE_HUB_CHALLENGE)
    ,curl(NULL)
  {
  }

  void InstagramConnection::send(const char* data, size_t len) {
    uv_buf_t buf;
    buf.base = (char*)data;
    buf.len = len;
    if(uv_write(&write_req, (uv_stream_t*)client, &buf, 1, instagram_after_write)) {
    }
  }

  // ---------------------------------------------------------
  // INSTAGRAM
  // ---------------------------------------------------------
  Instagram::Instagram()
    :must_stop_idler(false)
    ,ssl_ctx(NULL)
  {
    curl_global_init(CURL_GLOBAL_ALL);
    curlm = curl_multi_init();
    RAND_poll();
    SSL_library_init();
    SSL_load_error_strings();
    ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
    SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, instagram_ssl_verify_peer);
    SSL_CTX_set_info_callback(ssl_ctx, dummy_ssl_info_callback);
  }

  Instagram::~Instagram() {
    curl_multi_cleanup(curlm);
  }

  bool Instagram::setup(const std::string clientID, const std::string clientSecret, const std::string sslPrivateKeyFile) {
    /*
    std::string line = "GET /callback.html?hub.challenge=c3b01b1384914f2c92fdfd66704b8409&hub.mode=subscribe HTTP/1.1";
    struct slre reg;
    struct cap capt[4 + 1];
    if(!slre_compile(&reg, "GET /callback.html\\?hub.challenge=(\\S+)&hub.mode")) {
      printf("ERROR: cannot compile regexp\n");
      return 0;
    }
    if(slre_match(&reg, line.c_str(), line.size(), capt)) {
      printf("Capt: %d, %s, %d\n", capt[1].len, capt[1].ptr, capt[0].len);
      char* res = new char[capt[1].len];
      memcpy(res, line.c_str()+33, capt[1].len);
      printf(">> %s\n", res);
    }
    else {
      printf("NO\n");
    }
    */

    client_id = clientID;
    client_secret = clientSecret;
    loop = uv_default_loop();
    server.data = this;
    idler.data = this;

    uv_tcp_init(loop, &server);

    // SSL
    int rc = SSL_CTX_use_PrivateKey_file(ssl_ctx, sslPrivateKeyFile.c_str(), SSL_FILETYPE_PEM);
    if(!rc) {
      printf("ERROR: Could not load client key file.\n");
      return false;
    }

    struct sockaddr_in bind_addr = uv_ip4_addr("0.0.0.0", 7000);
    uv_tcp_bind(&server, bind_addr);
    int r = uv_listen((uv_stream_t*)&server, 128, instagram_on_new_connection);
    if(r) {
      printf("ERROR: listen error: %s\n", uv_err_name(uv_last_error(loop)));
      return false;
    }

    return true;
  }

  void Instagram::subscribe(const std::string object, const std::string objectID, const std::string aspect, const std::string token) {
    printf("TODO TODO: CREATING A HTTPREQUEST ON THE HEAD W/O TRACKING IT!!!!!\n");
    HTTPRequest* req = new HTTPRequest(loop, ssl_ctx);
    req->setURL("api.instagram.com","/v1/subscriptions/", true, true);
    //req->setURL("test.localhost","/test.php", false, true);

    
    req->addFormField("client_id", client_id);
    req->addFormField("client_secret", client_secret);
    req->addFormField("object", object);
    req->addFormField("object_id", objectID);
    req->addFormField("aspect", aspect);
    req->addFormField("token", token);
     req->addFormField("callback_url", "http://home.roxlu.com:7000/callback.html");
    //   req->addFormField("callback_url", "http://home.roxlu.com/callback.html");
    
    req->send();
  }

  void Instagram::sendHTTP(const std::string data) {
  }

  void Instagram::update() {
    uv_run_once(loop);
  }

} // roxlu
