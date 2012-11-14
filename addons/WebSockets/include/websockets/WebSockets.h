#ifndef ROXLU_WEBSOCKETSH
#define ROXLU_WEBSOCKETSH

extern "C" {
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
}

#include <string>
#include <vector>
#include <map>
#include <algorithm>

enum WebSocketStates {
  WS_STATE_NONE
  ,WS_STATE_HANDSHAKE_RECEIVE
  ,WS_STATE_HANDSHAKE_VERIFY
  ,WS_STATE_OPEN
};

enum WebSocketFrameStates {
  F_STATE_HEADER
  ,F_STATE_HEADER_LEN_1
  ,F_STATE_HEADER_LEN_2
  ,F_STATE_HEADER_LEN_8
  ,F_STATE_MASKING_KEY
  ,F_STATE_APP_DATA
};

enum WebSocketFrameOpCodes {
  F_OPCODE_CONTNUATION_FRAME = 0x00
  ,F_OPCODE_TEXT_FRAME = 0x01
  ,F_OPCODE_BINARY_FRAME = 0x02
  ,F_OPCODE_RESERVED = 0x03 // actually from 0x03-0x07
  ,F_OPCODE_CLOSE = 0x08
  ,F_OPCODE_PING = 0x09
  ,F_OPCODE_PONG = 0xA
};

enum HTTPRequestStates {
  HS_STATE_NONE
  ,HS_STATE_PARSE_GET // we still need to parse the 'GET' line.
  ,HS_STATE_PARSE_HEADER // we parsed the GET line; now parsing line by line.
};


struct WebSocketHTTPHeader {
  std::string name;
  std::string value;
};

struct WebSocketHTTPRequest {
  WebSocketHTTPRequest();
  ~WebSocketHTTPRequest();
  void addHeader(const char* name, const char* value);
  bool findHeader(const char* name, std::string& result);
  void reset(); // clear all used memory 
  void print();

  std::map<std::string, WebSocketHTTPHeader> headers;
  bool has_all_headers;
  int state;
  bool has_websocket_key;
  void copyToBuffer(evbuffer* buffer);
};

struct WebSocketFrame {
  WebSocketFrame();
  ~WebSocketFrame();
  bool hasBytes() { return dx < buffer.size(); } 
  size_t getBytesLeftToRead() { return buffer.size() - dx; }
  void setData(const char* data, size_t len);
  size_t getSize() { return buffer.size(); }
  bool isBinary();
  
  std::string getAppDataAsText();

  const char* getPtr() { return (const char*)&buffer[0]; }
  const char* getAppDataPtr() { return (const char*)getPtr()+app_data_dx; }
  ev_uint64_t getPayloadLen() { return payload_len; }

  std::vector<ev_uint8_t> buffer;
  int state;
  size_t dx;
  size_t app_data_dx;
  ev_uint64_t payload_len;
  ev_uint8_t opcode;
  ev_uint8_t mask_key[4];
  int mask_key_index;
  bool is_masked;
  bool is_final;
  bool has_rsv;
};


class WebSockets;
struct WebSocketConnection {
  WebSocketConnection();
  ~WebSocketConnection();
  void addToOutputBuffer(const char* data, size_t len);
  WebSockets* ws;
  bufferevent* bev;
  int state;
  WebSocketHTTPRequest request;
  WebSocketFrame* frame; // used to parse buffer.
  std::vector<WebSocketFrame*> frames;
};

class WebSocketListener {
public:
  virtual void onNewConnection(WebSocketConnection* c) = 0;
  virtual void onNewFrame(WebSocketConnection* c, WebSocketFrame* frame) = 0;
};

class WebSockets {
public:
  WebSockets();
  ~WebSockets();
  bool setup();
  void update();

  static void callbackAcceptNewConnection(
                                          evconnlistener* listener
                                          ,evutil_socket_t fd
                                          ,struct sockaddr* address
                                          ,int socklen
                                          ,void* ctx
                                          );

  static void callbackAcceptError(
                                  evconnlistener* listener
                                  ,void* ctx
                                  );

  static void callbackRead(
                           bufferevent* bev
                           ,void* ctx
                           );
  
  static void callbackEvent(
                            bufferevent* bev
                            ,short events
                            ,void* ctx
                            );
  
  void sendHTTPRequest(WebSocketConnection* c, WebSocketHTTPRequest& req);
  void sendToAllClients(const char* buf, size_t len, bool binary = false);
  void setListener(WebSocketListener* l);
private:
  void parseBuffer(WebSocketConnection* c);
  int createSecurityKey(const char* clientKey, char* result, int size);
  void removeConnection(WebSocketConnection* c);
private:
  WebSocketListener* ws_listener;
  event_base* evbase;
  evconnlistener* ev_listener;
  sockaddr_in sin;
  int state;
  std::vector<WebSocketConnection*> connections;
};

inline void WebSockets::setListener(WebSocketListener* l) {
  ws_listener = l;
}

#endif

