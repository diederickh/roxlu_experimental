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

  void instagram_http_request_close(HTTPRequest* req, void* user) {
    // call close handler if given.
    InstagramAPICallback* api = static_cast<InstagramAPICallback*>(user);
    if(api->close_cb) {
      api->close_cb(req->in_buffer.c_str(), req->in_buffer.size(), api->user);
    }

    // remove request + api callback from instagram context
    Instagram* ins = api->instagram;
    std::vector<HTTPRequest*>::iterator it = std::find(ins->requests.begin(), ins->requests.end(), req);
    if(it != ins->requests.end()) {
      ins->requests.erase(it); // @todo where do we delete this?
    }
    std::vector<InstagramAPICallback*>::iterator ait = std::find(ins->api_callbacks.begin(), ins->api_callbacks.end(), api);
    if(ait != ins->api_callbacks.end()) {
      ins->api_callbacks.erase(ait); // @todo where do we delete this 
    }
  }

  // ---------------------------------------------------------
  // Libuv Callbacks
  // ---------------------------------------------------------
  uv_buf_t instagram_alloc_buffer(uv_handle_t* buffer, size_t suggestedSize) {
    return uv_buf_init((char*)malloc(suggestedSize), suggestedSize);
  }

  void instagram_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
    InstagramConnection* con = static_cast<InstagramConnection*>(stream->data);
    
    // CLOSED
    if(nread < 0) {
      if(buf.base) {
        free(buf.base);
        buf.base = NULL;
      }
      con->state = ICS_NONE;
      // ---------------------------
      // HANDLE SUBSCRIPTION DATA
      // -----------------------------
      if(con->buffer[0] == 'P' && con->buffer[1] == 'O' && con->buffer[2] == 'S' && con->buffer[3] == 'T') {
        std::stringstream ss(con->buffer);
        std::string line;
        bool last_line = false;
        size_t size = con->buffer.size();
        size_t body = 0;
        for(int i = 0; i < con->buffer.size(); ++i) {
          if(con->buffer[i] == '\r' && con->buffer[i+1] == '\n' && con->buffer[i+2] == '\r' && con->buffer[i+3] == '\n') {
            body = i+4;
            break;
          }
        }

        while(std::getline(ss, line)) {
          if(last_line) {
            printf(">>>>>%s<<<<<\n", line.c_str());
          }
          if(line[0] == '\r' && line[1] == '\n') {
            last_line = true;
          }
        }
      }

      // DELETE CONNECTION
      std::vector<InstagramConnection*>::iterator it = std::find(con->instagram->clients.begin(), con->instagram->clients.end(), con);
      if(it != con->instagram->clients.end()) {
        con->instagram->clients.erase(it);
      }
     //	@TODO DELETE CONNECTION!!
      return;
    }
    // ERROR (?)
    else if(nread == 0) {
      printf("@TODO WE MIGHT NEED TO REMOVE THE REQUEST HERE TOO!\n");
      free(buf.base);
      buf.base = NULL;
      return;
    }
    else {
      std::copy(buf.base, buf.base+nread, std::back_inserter(con->buffer));
      free(buf.base);
      buf.base = NULL;
      if(con->buffer.size() < 3) {
        return;
      }
      std::stringstream ss(con->buffer);
      
      // ---------------------------
      // HANDLE SUBSCRIPTION REQUEST
      // -----------------------------
      if(con->buffer[0] == 'G' && con->buffer[1] == 'E' && con->buffer[2] == 'T') {
        struct slre reg;
        struct cap capt[4 + 1];
        int mode = 0; // mode 1: subscribe callback, mode 2: oauth token exchange

        // callback.html
        if(!slre_compile(&reg, "GET /callback.html\\?hub.challenge=(\\S+)&hub.mode")) { // we need to escape the ?
          printf("WARNING: cannot compile regexp for callback.html\n");
          return;
        }
        if(mode == 0 && slre_match(&reg, con->buffer.c_str(), con->buffer.size(), capt)) {
          mode = 1;
        }
        
        // oauth.html
        if(mode == 0 && con->buffer.find("GET /oauth.html HTTP/1.1") != std::string::npos) {
          mode = 2;
        }
                
        if(mode == 1) {
          // SUBSCRIBE CALLBACK
          char* res = new char[capt[1].len];
          memcpy(res, con->buffer.c_str()+33, capt[1].len);
          std::stringstream http;
          http << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << capt[1].len << "\r\n"
               << "\r\n"
               << res;
          con->send(http.str().c_str(), http.str().size());
          delete[] res;
        }
        else if(mode == 2) {
          // OAUTH CALLBACK
          std::string msg = "<div style='font-family:arial;padding:10;background-color:#FFC;font-size:15px'>Copy the <strong style='color:red'>access_token</strong> value from the URL and save it to a file.<br>Use Instagram::setAccessToken() to enable authenticated requests for your application.</div>";
          std::stringstream http;
          http << "HTTP/1.1 200 OK\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << msg.size() << "\r\n"
               << "Connction: close\r\n"
               << "\r\n"
               << msg;
          con->send(http.str().c_str(), http.str().size());
        }
      }
    }
   }

   void instagram_after_write(uv_write_t* req, int status) {
   }

   void instagram_on_new_connection(uv_stream_t* server, int status) {
     if(status == -1) {
       printf("ERROR: cannot accept new connection.\n");
       return;
     }

     Instagram* instagram = static_cast<Instagram*>(server->data);
     InstagramConnection* con = new InstagramConnection(instagram);
     con->client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t)); // @todo FREE
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
     ,is_setup(false)
   {
     RAND_poll();
     SSL_library_init();
     SSL_load_error_strings();
     ssl_ctx = SSL_CTX_new(SSLv23_client_method());
     SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
     SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, instagram_ssl_verify_peer);
     //SSL_CTX_set_info_callback(ssl_ctx, dummy_ssl_info_callback);
   }

   Instagram::~Instagram() {
   }

   bool Instagram::setup(const std::string clientID, const std::string clientSecret, const std::string sslPrivateKeyFile) {
     oauth.setup(clientID, clientSecret);
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
     is_setup = true;
     return true;
   }
  
  void Instagram::requestAccessToken() {
    HTTPParams qs;
    qs.add("client_id", client_id);
    qs.add("redirect_uri", "http://home.roxlu.com:7000/oauth.html");
    qs.add("response_type", "token");
    qs.percentEncode();
    
    std::stringstream ss;
    ss << "https://instagram.com/oauth/authorize/?" << qs.getQueryString();
    printf("\n------------------------------------------------------------------\n");
    printf("Open this link in a browser:\n%s\n", ss.str().c_str());
    printf("\n------------------------------------------------------------------\n");
  }


   // @todo set on data callback to check response.
   void Instagram::subscribe(const std::string object, const std::string objectID, const std::string aspect, const std::string token) {
     HTTPRequest* req = new HTTPRequest(loop, ssl_ctx, NULL, NULL, instagram_http_request_close, this);
     requests.push_back(req);
     req->setURL("api.instagram.com","/v1/subscriptions/", true, true);
     req->addFormField("client_id", client_id);
     req->addFormField("client_secret", client_secret);
     req->addFormField("object", object);
     req->addFormField("object_id", objectID);
     req->addFormField("aspect", aspect);
     req->addFormField("token", token);
     req->addFormField("callback_url", "http://home.roxlu.com:7000/callback.html");
     req->send();
   }

  void Instagram::subscriptions(cb_instagram_on_data dataCB, cb_instagram_on_close closeCB, void* user) {
    InstagramAPICallback* api = new InstagramAPICallback(closeCB, user, this);
    HTTPRequest* req = new HTTPRequest(loop, ssl_ctx, dataCB, user, instagram_http_request_close, api);
    requests.push_back(req);
    api_callbacks.push_back(api);
    req->setURL("api.instagram.com", "/v1/subscriptions?client_secret=" +client_secret +"&client_id=" +client_id, false, true);
    req->send();
  }

  void Instagram::mediaPopular(cb_instagram_on_data dataCB, cb_instagram_on_close closeCB, void* user) {
    InstagramAPICallback* api = new InstagramAPICallback(closeCB, user, this);
    HTTPRequest* req = new HTTPRequest(loop, ssl_ctx, dataCB, user, instagram_http_request_close, api);
    requests.push_back(req);
    api_callbacks.push_back(api);
    req->setURL("api.instagram.com", "/v1/media/popular", false, true);
    req->addURLField("access_token", oauth.getToken());
    req->send();
  }

  void Instagram::locationsSearch(HTTPParams& params, cb_instagram_on_data dataCB, cb_instagram_on_close closeCB, void* user) {
    InstagramAPICallback* api = new InstagramAPICallback(closeCB, user, this);
    HTTPRequest* req = new HTTPRequest(loop, ssl_ctx, dataCB, user, instagram_http_request_close, api);
    requests.push_back(req);
    api_callbacks.push_back(api);
    req->url_fields.copy(params);
    req->setURL("api.instagram.com", "/v1/locations/search", false, true);
    req->addURLField("access_token", oauth.getToken());
    req->send();
  }

  // http://instagram.com/developer/endpoints/media/#get_media_search
  void Instagram::mediaSearch(HTTPParams& params, cb_instagram_on_data dataCB, cb_instagram_on_close closeCB, void* user) {
    InstagramAPICallback* api = new InstagramAPICallback(closeCB, user, this);
    HTTPRequest* req = new HTTPRequest(loop, ssl_ctx, dataCB, user, instagram_http_request_close, api);
    requests.push_back(req);
    api_callbacks.push_back(api);
    req->url_fields.copy(params);
    req->setURL("api.instagram.com", "/v1/media/search", false, true);
    req->addURLField("access_token", oauth.getToken());
    req->send();
  }

  void Instagram::setAccessToken(const std::string token) {
    oauth.setToken(token);
  }

  void Instagram::update() {
    uv_run_once(loop);
  }

} // roxlu
