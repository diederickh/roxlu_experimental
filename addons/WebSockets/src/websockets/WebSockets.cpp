#include <websockets/WebSockets.h>
#include <websockets/extern/Base64Encoder.h>

extern "C" {
#include <websockets/extern/sha1.h>
}

// LibEvent callbacks. 
// ------------------------------------------------------------------------------------
void WebSockets::callbackAcceptNewConnection(
                                             evconnlistener* listener
                                             ,evutil_socket_t fd
                                             ,struct sockaddr* address
                                             ,int socklen
                                             ,void* ctx
                                             )
{
  printf("callbackAcceptNewConnection()\n");
  WebSockets* ws = static_cast<WebSockets*>(ctx);
  WebSocketConnection* con = new WebSocketConnection();
  ws->connections.push_back(con);
  con->state = WS_STATE_HANDSHAKE_RECEIVE;
  con->ws = ws;
  con->bev = bufferevent_socket_new(ws->evbase, fd, BEV_OPT_CLOSE_ON_FREE);

  bufferevent_setcb(con->bev, &WebSockets::callbackRead, NULL, callbackEvent, con);
  bufferevent_enable(con->bev, EV_READ | EV_WRITE);

  if(ws->ws_listener != NULL) {
    ws->ws_listener->onNewConnection(con);
  }
}

void WebSockets::callbackAcceptError(
                                     evconnlistener* listener
                                     ,void* ctx
                                     )
{
  printf("callbackAcceptError()\n");
}

void WebSockets::callbackRead(
                              bufferevent* bev
                              ,void* ctx
                              )
{
  printf("callbackRead()\n");
  WebSocketConnection* wc = static_cast<WebSocketConnection*>(ctx);
  wc->ws->parseBuffer(wc);
}

void WebSockets::callbackEvent(
                               bufferevent* bev
                               ,short events
                               ,void* ctx
                               )
{
  printf("callbackEvent()\n");
  if(events & BEV_EVENT_ERROR) {
    printf("ERROR: We received an error from libevent.\n");
  }
  if(events & BEV_EVENT_EOF) {
    WebSocketConnection* c = static_cast<WebSocketConnection*>(ctx);
    c->ws->removeConnection(c);
  }
}


// WebSockets
// ------------------------------------------------------------------------------------
WebSockets::WebSockets() 
  :evbase(NULL)
  ,ev_listener(NULL)
  ,ws_listener(NULL)
  ,state(WS_STATE_NONE)
{

}

WebSockets::~WebSockets() {
  if(evbase != NULL) {
    event_base_free(evbase);
  }
  for(std::vector<WebSocketConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    delete *it;
  }
}

bool WebSockets::setup() {
  event_enable_debug_mode();
  evbase = event_base_new();
  int port = 8080;
  
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(0);
  sin.sin_port = htons(port);

  ev_listener = evconnlistener_new_bind(
                                     evbase
                                     ,&WebSockets::callbackAcceptNewConnection
                                     ,this
                                     ,LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE
                                     ,-1
                                     ,(struct sockaddr*)&sin
                                     ,sizeof(sin)
                                     );
  if(!ev_listener) {
    printf("ERROR: Couldn't create listener.\n");
    return false;
  }
  evconnlistener_set_error_cb(ev_listener, &WebSockets::callbackAcceptError);
  return true;
}

void WebSockets::update() {
  event_base_loop(evbase, EVLOOP_NONBLOCK);
}

void WebSockets::parseBuffer(WebSocketConnection* c) {
  printf("parseBuffer();\n");
  evbuffer* input = bufferevent_get_input(c->bev);
  switch(c->state) {

    // We received a new fresh request; parse http headers and find websocket key.
  case WS_STATE_HANDSHAKE_RECEIVE: {
    size_t len;
    char* line; 
    while(!c->request.has_all_headers && (line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF))) {
      if(len == 0) {
        c->request.has_all_headers = true;
        if(c->request.has_websocket_key) {
          c->state = WS_STATE_HANDSHAKE_VERIFY;
          parseBuffer(c);
        }
        else {
          printf("ERROR: invalid headers. @todo disconnect.\n");
        }
        break;
      }
      else {
        if(c->request.state == HS_STATE_PARSE_GET && len > 3) {
          if( (line[0] == 'G' || line[0] == 'g') && (line[1] == 'E' || line[1] == 'e') && (line[2] == 'T' || line[2] == 't')) {
            c->request.state = HS_STATE_PARSE_HEADER;
          }
        }
        else if(len > 0 && c->request.state == HS_STATE_PARSE_HEADER) {
          int name = 1;
          WebSocketHTTPHeader h;
          for(int i = 0; i < len; ++i) {
            if(line[i] == ':') {
              std::transform(h.name.begin(), h.name.end(), h.name.begin(), ::tolower);
              name = 0;
              continue;
            }
            if(name) {
              h.name.push_back(line[i]);
            }
            else {
              if(h.value.size() == 0 && line[i] == ' ') {
              }
              else {
                h.value.push_back(line[i]);
              }
            }
          }
          if(h.name == "sec-websocket-key") {
            c->request.has_websocket_key = true;
          }
          c->request.headers[h.name] = h;
        };
        delete line;
      }
    }
    break;
  }
    // Once connected we verify the websocket handshake
  case WS_STATE_HANDSHAKE_VERIFY: {
    std::string websocket_key;
    if(!c->request.findHeader("sec-websocket-key", websocket_key)) {
      printf("ERROR: Cannot verify handshake. @todo disconnect\n");
      return;
    }
    std::string upgrade;
    if(!c->request.findHeader("upgrade", upgrade)) {
      printf("ERROR: Cannot find upgrade header. @todo disconnect\n");
      return;
    }
    char websocket_key_result[1024];
    int r = createSecurityKey(websocket_key.c_str(), websocket_key_result, sizeof(websocket_key_result));
    if(r < 0) {
      printf("ERROR: Cannot create reply websocket key. @todo disconnect\n");
      return;
    }
    WebSocketHTTPRequest response;
    response.addHeader("Upgrade", "websocket");
    response.addHeader("Connection", "Upgrade");
    response.addHeader("Sec-WebSocket-Accept", websocket_key_result);
    sendHTTPRequest(c, response);
    c->state = WS_STATE_OPEN;
    c->request.reset(); // reset the in request.
    parseBuffer(c);
    break;
  };

    // We are connected + verified, parse WebSocket frames.
  case WS_STATE_OPEN: {
    if(c->frame == NULL) {
      c->frame = new WebSocketFrame();
    }

    int copied = 0;
    ev_uint8_t tmp[1024 * 50];
    do { 
      copied = evbuffer_remove(input, tmp, sizeof(tmp));
      std::copy(tmp, tmp+copied, std::back_inserter(c->frame->buffer));
    } while(copied > 0);


    while(c->frame->hasBytes()) {
      switch(c->frame->state) {

        // FIN, RSV, OPCODE
      case F_STATE_HEADER: {
        ev_uint8_t b =  c->frame->buffer[c->frame->dx];
        c->frame->is_final = b & 0x80;
        c->frame->has_rsv = ((b & 0x70) != 0);
        if(c->frame->has_rsv) {
          printf("ERROR: We did not implement extensions.\n");
          return;
        }
        c->frame->opcode = (b & 0x0F);
        c->frame->state = F_STATE_HEADER_LEN_1;
        c->frame->dx = 1;

        printf("RECEIVED, FIRST BYTE: %02X, OPCODE: %02X, IS_FINAL: %d, HAS_RSV: %d\n", b, c->frame->opcode, c->frame->is_final, c->frame->has_rsv);
        break;
      };
        
        // GET THE LENGTH OF THE PAYLOAD
      case F_STATE_HEADER_LEN_1: {
        ev_uint8_t b = c->frame->buffer[c->frame->dx];
        c->frame->dx++;
        c->frame->is_masked = b & 0x80; 
        if(!c->frame->is_masked) {
          printf("ERROR: All client data must be masked!\n");
          return;
        }
        printf("MASKED: %d\n", c->frame->is_masked);

        c->frame->payload_len = (b & 0x7F);
        if(c->frame->payload_len < 126) {
          c->frame->state = F_STATE_MASKING_KEY;
          continue;
        }
        else if(c->frame->payload_len == 126){
          c->frame->state = F_STATE_HEADER_LEN_2;
          continue;
        }
        else if(c->frame->payload_len == 127) {
          c->frame->state = F_STATE_HEADER_LEN_8;
          continue;
        }
        break;
      };

      case F_STATE_HEADER_LEN_2: {
        if(c->frame->getBytesLeftToRead() < 2) {
          return;
        }
        // @todo check endian
        c->frame->payload_len = ((ev_uint16_t)c->frame->buffer[c->frame->dx]) << 8 | c->frame->buffer[c->frame->dx+1];
        c->frame->dx += 2;
        c->frame->state = F_STATE_MASKING_KEY;
        continue;
      }

      case F_STATE_HEADER_LEN_8: {
        if(c->frame->getBytesLeftToRead() < 8) {
          printf("ERROR: Header_len_8, we do not have enough bytes in buffer yet.\n");
          return;
        }
        // @todo check big endian systems
        char* d = (char*)&c->frame->payload_len;
        for(int i = 0; i < 8; ++i) {
          ev_uint8_t b = c->frame->buffer[c->frame->dx];
          d[7-i] = b;
          c->frame->dx++;
        }
        c->frame->state = F_STATE_MASKING_KEY;
        continue;
      }

        // GET THE MASKING KEY
      case F_STATE_MASKING_KEY: {
        if(c->frame->is_masked) {
          memcpy((char*)&c->frame->mask_key, (char*)&c->frame->buffer[c->frame->dx], 4);
          c->frame->state = F_STATE_APP_DATA;
          c->frame->dx += 4;
          c->frame->app_data_dx = c->frame->dx;
          continue;
        }
        else {
          printf("ERROR: Client data should be masked. @todo disconnect.\n");
          return;
        }
        break;
      };

        // HANDLE APP DATA
      case F_STATE_APP_DATA: {
        size_t app_data_read =  (c->frame->dx - c->frame->app_data_dx); 
        size_t app_data_received = c->frame->buffer.size() - c->frame->app_data_dx;
        /*
        printf("App_Data_Read: %zu, App_Data_Index: %zu, Current_Index: %zu, Payload_Len: %lld, App_Data_Received: %zu\n"
               ,app_data_read
               ,c->frame->app_data_dx
               ,c->frame->dx
               ,c->frame->payload_len
               ,app_data_received
               );
        */
        if(app_data_received >= c->frame->payload_len) {
          ///printf(">>>> WE HAVE RECEIVED ALL PAYLOAD DATA, CONVERT!, STATE: %d / %d,\n", c->state, WS_STATE_OPEN);
          for(int i = 0; i < c->frame->payload_len; ++i) {
              c->frame->buffer[c->frame->dx]  = c->frame->buffer[c->frame->dx] ^ c->frame->mask_key[c->frame->mask_key_index];
              c->frame->mask_key_index = ++c->frame->mask_key_index % 4;
              c->frame->dx++;
          }
          c->frames.push_back(c->frame);  
          if(ws_listener != NULL) {
            ws_listener->onNewFrame(c, c->frame);
          }
          //printf("Left to rad %zu\n", c->frame->getBytesLeftToRead());
          WebSocketFrame* new_frame = new WebSocketFrame();
          if(c->frame->getBytesLeftToRead()) {
            std::copy(
                      c->frame->getPtr()+c->frame->dx
                      ,c->frame->getPtr() + c->frame->dx + c->frame->getBytesLeftToRead()
                      ,std::back_inserter(new_frame->buffer)
                      );
          }
          c->frame = new_frame;
          printf("--------- READY WITH FRAME ------------\n");
        }
        return; // @todo, this might cause bugs
        break;
      };
      default: printf("ERROR: Unhandled Frame State\n"); break;
      }; // switch(c->frame->state)
    } // while (c->frame->hasBytes())
    break; 
  }; // WS_STATE_OPEN
  default:break;
  } // switch
}

void WebSockets::sendHTTPRequest(WebSocketConnection* c, WebSocketHTTPRequest& req) {
    evbuffer* output = bufferevent_get_output(c->bev);
    evbuffer_add_printf(output, "%s", "HTTP/1.1 101 Web Socket Protocol HandShake\r\n");
    req.copyToBuffer(output); // libev sends the data as soon as possible after adding/copying.
}

void WebSockets::sendToAllClients(const char* buffer, size_t len, bool binary) {
  printf("sendToAllClients(): '%s', len: %zu\n", buffer, len);
  WebSocketFrame frame;
  frame.is_final = true;
  frame.has_rsv = false;
  frame.is_masked = false;
  frame.opcode = (binary) ? F_OPCODE_BINARY_FRAME : F_OPCODE_TEXT_FRAME;
  frame.payload_len = len;
  frame.setData(buffer, len);
  
  for(std::vector<WebSocketConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    (*it)->addToOutputBuffer(frame.getPtr(), frame.getSize());
  }
}

// result must be of SHA1HashSize size
int WebSockets::createSecurityKey(const char* clientKey, char* result, int size) {
  char buf[1024];
  sprintf(buf, "%s%s", clientKey, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
  SHA1Context sha_ctx;
  int r = SHA1Reset(&sha_ctx);
  if(r != shaSuccess) {
    printf("ERROR: cannot reset sha1 context.\n");
    return -1;
  }

  r = SHA1Input(&sha_ctx, (const uint8_t*)buf, strlen(buf));
  if(r != shaSuccess) {
    printf("ERROR: cannot set contents for sha1.\n");
    return -1;
  }

  uint8_t msg_digest[SHA1HashSize];
  r = SHA1Result(&sha_ctx, msg_digest);
  if(r != shaSuccess) {
    printf("ERROR: cannot create sha digest.\n");
    return -1;
  }

  aoBase64Encoder b64;
  b64.Encode((const unsigned char*)msg_digest, SHA1HashSize);

  const char* encoded_data = b64.GetEncoded();
  int s = b64.GetEncodedSize();
  if(s > size) {
    printf("ERROR: the buffer  you passed to createSecurityKey is not big enough, should be: %d\n", s);
    return -1;
  }

  for(int i = 0; i < s; ++i) {
    result[i] = encoded_data[i];
  }
  result[s] = '\0';
  return s;
}

void WebSockets::removeConnection(WebSocketConnection* c) {
  for(std::vector<WebSocketConnection*>::iterator it = connections.begin(); it != connections.end(); ++it) {
    if((*it) == c) {
      delete c;
      connections.erase(it);
      return;
    }
  }
}


// WebSocketConnection
// ------------------------------------------------------------------------------------
WebSocketConnection::WebSocketConnection()
  :ws(NULL)
  ,bev(NULL)
  ,state(WS_STATE_NONE)
  ,frame(NULL)
{
}

WebSocketConnection::~WebSocketConnection() {
  printf("~WebSocketConnection()\n");
  for(std::vector<WebSocketFrame*>::iterator it = frames.begin(); it != frames.end(); ++it) {
    delete *it;
  }
}

void WebSocketConnection::addToOutputBuffer(const char* data, size_t len) {
    evbuffer* output = bufferevent_get_output(bev);
    evbuffer_add(output, data, len);
}

// HTTPRequest
// ------------------------------------------------------------------------------------
WebSocketHTTPRequest::WebSocketHTTPRequest()
    :has_all_headers(false)
    ,state(HS_STATE_PARSE_GET)
    ,has_websocket_key(false)
{

}

WebSocketHTTPRequest::~WebSocketHTTPRequest() {
}

void WebSocketHTTPRequest::print() {
  for(std::map<std::string, WebSocketHTTPHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
    printf("> %s: %s\n", it->first.c_str(), it->second.value.c_str());
  }
}

void WebSocketHTTPRequest::addHeader(const char* name, const char* value) {
  WebSocketHTTPHeader h;
  h.name.append(name, strlen(name));
  h.value.append(value, strlen(value));
  headers[h.name] = h;
}

bool WebSocketHTTPRequest::findHeader(const char* name, std::string& value) {
  std::map<std::string, WebSocketHTTPHeader>::iterator it = headers.find(name);
  if(it == headers.end()) {
    return false;
  }
  value = it->second.value;
  return true;
}

void WebSocketHTTPRequest::copyToBuffer(evbuffer* b) {
    for(std::map<std::string, WebSocketHTTPHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
      evbuffer_add(b, it->first.c_str(), it->first.size());
      evbuffer_add(b, ": ", 2);
      evbuffer_add(b, it->second.value.c_str(), it->second.value.size());
      evbuffer_add(b, "\r\n", 2);
    }
    evbuffer_add(b, "\r\n", 2);
}

void WebSocketHTTPRequest::reset() {
  headers.clear();
  state = HS_STATE_NONE;
  has_websocket_key = false;
}

// Frame
// ------------------------------------------------------------------------------------
WebSocketFrame::WebSocketFrame()
  :state(F_STATE_HEADER)
  ,dx(0)
  ,is_masked(false)
  ,is_final(0)
  ,opcode(0)
  ,has_rsv(false)
  ,payload_len(0)
  ,mask_key_index(0)
{
  memset((char*)mask_key, 0, 4);
};

WebSocketFrame::~WebSocketFrame() {
  printf("WebSocketFrame::~WebSocketFrame()\n");
}

// Create the raw frame buffer that we can send. you need to set the members yourself
void WebSocketFrame::setData(const char* data, size_t len) {
  // BYTE 0
  ev_uint8_t fin_bit = (is_final) ? 0x80 : 0x00;
  ev_uint8_t rsv_bits = (has_rsv) ? 0x00 : 0x00; // @todo implement extensions
  ev_uint8_t opcode_bits = opcode;
  buffer.push_back(fin_bit | rsv_bits | opcode_bits);

  // BYTE 1
  ev_uint8_t mask_bit = (is_masked) ? 0x80 : 0x00;
  if(payload_len < 126) {
    ev_uint8_t size_bits = 0x7F & len;
    buffer.push_back(size_bits | mask_bit);
  }
  else {
    printf("ERROR: We have not yet implemented frames which are bigger then 125 bytes");
  }

  // Copy data
  std::copy(data, data+len, std::back_inserter(buffer));
}

std::string WebSocketFrame::getAppDataAsText() {
  std::string str(getPtr()+app_data_dx, payload_len);
  return str;
}

bool WebSocketFrame::isBinary() {
  return opcode == F_OPCODE_BINARY_FRAME;
}

/* Example conversation:
// ------------------------------------------------------------------------------------
--
GET /?encoding=text HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Host: echo.websocket.org
Origin: http://www.websocket.org
Sec-WebSocket-Key: QYCX46Vo1LhlK4OsIvUo/A==
Sec-WebSocket-Version: 13
Sec-WebSocket-Extensions: x-webkit-deflate-frame
Cookie: __utma=9925811.355707020.1350675536.1350684450.1350829433.3; __utmb=9925811.2.10.1350829433; __utmc=9925811; __utmz=9925811.1350829433.3.3.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=(not%20provided)

HTTP/1.1 101 Web Socket Protocol Handshake
Upgrade: WebSocket
Connection: Upgrade
Sec-WebSocket-Accept: zZ+40pNtayZ+q1wdeb7ppswx2QY=
Server: Kaazing Gateway
Date: Sun, 21 Oct 2012 14:38:38 GMT
Access-Control-Allow-Origin: http://www.websocket.org
Access-Control-Allow-Credentials: true
Access-Control-Allow-Headers: content-type
Access-Control-Allow-Headers: authorization
Access-Control-Allow-Headers: x-websocket-extensions
Access-Control-Allow-Headers: x-websocket-version
Access-Control-Allow-Headers: x-websocket-protocol

..}&../I..]O..
O..]n..1....D...M....Rock it with HTML5 WebSocket..
..._...-...z...-...A...h...n.....Rock it with HTML5 WebSocket
--
 */




