#ifndef ROXLU_HTTP_REQUESTH
#define ROXLU_HTTP_REQUESTH

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

extern "C" {
#include <event2/dns.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent_ssl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
}


#include <twitter/Buffer.h>
#include <twitter/PercentEncode.h>
#include <twitter/Types.h>

// TYPES
// -------
enum HTTPTypes {
  REQUEST_POST
  ,REQUEST_GET
  ,HTTP10
  ,HTTP11
  ,HTTP_OK = 200  
};

enum HTTPConnectionStates {
  HS_NONE
};

enum HTTPParseStates {
  HPS_METHOD
  ,HPS_HEADERS
  ,HPS_BODY
};

// HTTP PARAMETER
struct HTTPParameter {
  HTTPParameter();
  HTTPParameter(const std::string name, const std::string val);
  std::string getString();
  void print();
  std::string name;
  std::string value;
};

struct HTTPParameters {
  template<class T>
  void add(const std::string name, T v) {
    std::stringstream ss;
    ss << v;
    HTTPParameter p(name, ss.str());
    add(p);
  }

  void add(const HTTPParameter p);
  void copy(const HTTPParameters& other);
  bool exist(const std::string name);
  size_t createFromVariableString(std::string str); // name=value&other=othervalue
  void clear();

  HTTPParameter& operator[](const std::string name);

  std::string getQueryString();
  void print();
  void percentEncode();
  std::map<std::string, HTTPParameter> entries;
};

// HTTP HEADERS 
// -------------
struct HTTPHeader {
  HTTPHeader();
  //  HTTPHeader(const std::string name, const std::string val);

  template<class T>
  HTTPHeader(const std::string n,  T val) {
    std::stringstream ss;
    ss << val;
    //ss >> value;
    value = ss.str();
    name = n;
    //printf("%s\n", ss.str().c_str());
  }

  std::string name;
  std::string value;
};

struct HTTPHeaders {
  void add(const std::string name, const std::string value);
  void add(const HTTPHeader h);
  std::string join(std::string nameValueSep = ": ", std::string lineEnd = "\r\n");
  std::map<std::string, HTTPHeader> entries;
};

// HTTP URL
struct HTTPURL {
  HTTPURL();
  HTTPURL(std::string proto, std::string host, std::string path);
  std::string getString();
  std::string proto;
  std::string host;
  std::string path;
};

// HTTP CONNECTION
//struct HTTPConnection {
//bufferevent* bev;
//};

// HTTP REQUEST
// -------------
class HTTPRequest {
public:
  HTTPRequest(int method = REQUEST_GET);
  ~HTTPRequest();

  void setURL(HTTPURL url);
  void setMethod(int method);
  void setVersion(int version);
  bool parseResponse(Buffer& b);
  std::string getBody(Buffer& b);

  HTTPURL getURL();
  std::string getQueryString();
  HTTPParameters getQueryStringParameters();
  HTTPParameters getContentParameters();
  int getMethod();

  void addHeader(HTTPHeader h);
  void addContentParameter(HTTPParameter p);
  void addQueryStringParameter(HTTPParameter p);
  void copyContentParameters(const HTTPParameters& p);

  std::string makeHTTPString();
  std::string makeRequestString();
public:
  int parse_state;
  int method;
  int version;
  int response_code;
  size_t body_start_dx;
  HTTPURL url;
  HTTPHeaders headers;
  HTTPParameters parameters;
  HTTPParameters querystring;
};


inline void HTTPRequest::setURL(HTTPURL u) {
  url = u;
}

inline void HTTPRequest::setMethod(int m) {
  method = m;
}

inline void HTTPRequest::setVersion(int v) {
  version = v;
}

inline HTTPURL HTTPRequest::getURL() {
  return url;
}

inline std::string HTTPRequest::getQueryString() {
  return "";
}

inline int HTTPRequest::getMethod() {
  return method;
}

inline HTTPParameters HTTPRequest::getContentParameters() {
  return parameters;
}

inline HTTPParameters HTTPRequest::getQueryStringParameters() {
  return querystring;
}


// HTTP CALLBACKS
struct HTTPConnection;
typedef void(*http_cb_on_read)(HTTPConnection* c, void* ctx);
typedef void(*http_cb_on_close)(HTTPConnection* c, void* ctx);
typedef void(*http_cb_on_error)(HTTPConnection* c, void* ctx);

// HTTP CONNECTION
class HTTP;
struct HTTPConnection {
  HTTPConnection(HTTP* http);
  ~HTTPConnection();
  void addToOutputBuffer(const std::string str);
  void addToOutputBuffer(const char* data, size_t size);
  bufferevent* bev;
  Buffer buffer;
  HTTPRequest response;
  int state; // used by "caller"
  HTTP* http;
  http_cb_on_close close_callback;
  http_cb_on_error error_callback;
  http_cb_on_read read_callback;
  void* close_callback_data;
  void* error_callback_data;
  void* read_callback_data;
#ifdef USE_OPENSSL
  SSL* ssl;
#endif
};

// HTTP CONTEXT
class HTTP {
public:
  HTTP();
  ~HTTP();
  void update();
  HTTPConnection* testSecure(SSL_CTX* sslContext);
  HTTPConnection* sendRequest(
                   HTTPRequest& r
                   ,http_cb_on_read readCB = NULL
                   ,void* readData = NULL
                   ,http_cb_on_close closeCB = NULL
                   ,void* closeData = NULL
                   ,http_cb_on_error errorCB = NULL
                   ,void* errorData = NULL
                   );

  HTTPConnection* sendSecureRequest(
                                    SSL_CTX* ctx
                                    ,HTTPRequest& r
                                    ,http_cb_on_read readCB = NULL
                                    ,void* readData = NULL
                                    ,http_cb_on_close closeCB = NULL
                                    ,void* closeData = NULL
                                    ,http_cb_on_error errorCB = NULL
                                    ,void* errorData = NULL
                                    );
  HTTPConnection* newConnection(    
                                http_cb_on_read readCB = NULL
                                ,void* readData = NULL
                                ,http_cb_on_close closeCB = NULL
                                ,void* closeData = NULL
                                ,http_cb_on_error errorCB = NULL
                                ,void* errorData = NULL
                                );

  //  static void callbackError(bufferevent* bev, short what, void* ctx);
  static void callbackRead(bufferevent* bev, void* ctx);
  static void callbackEvent(bufferevent* bev, short events, void* ctx);
  static void callbackLog(int severity, const char* msg);
private:
  void removeConnection(HTTPConnection* c);
  void removeAllConnections();
private:
  event_base* evbase;
  evdns_base* dnsbase;
  std::vector<HTTPConnection*> connections;
};
#endif
