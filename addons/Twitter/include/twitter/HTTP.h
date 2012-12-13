#ifndef ROXLU_HTTP_REQUESTH
#define ROXLU_HTTP_REQUESTH

#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

extern "C" {
#include <uv.h>
#include <http_parser.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
}

#include <twitter/Buffer.h>
#include <twitter/PercentEncode.h>
#include <twitter/Types.h>
#include <twitter/SSLBuffer.h>

namespace roxlu {
  namespace twitter {

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

      template<class T>
      HTTPHeader(const std::string n,  T val) {
        std::stringstream ss;
        ss << val;
        value = ss.str();
        name = n;
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


    // HTTP REQUEST
    // -------------
    typedef void(*cb_request_read_body)(const char* body, size_t len, void* userdata);

    class HTTPRequest {
    public:
      HTTPRequest(int method = REQUEST_GET);
      ~HTTPRequest();

      void setURL(HTTPURL url);
      void setMethod(int method);
      void setVersion(int version);
      void addToInputBuffer(const char* b, size_t len);
      bool parseInputBuffer(); // Response(); // Buffer& b); // const char* data, size_t len);
      void setReadBodyCallback(cb_request_read_body readCB, void* readData);
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

    private:
      static int parserCallbackOnMessageBegin(http_parser* p);
      static int parserCallbackOnURL(http_parser* p, const char* at, size_t len);
      static int parserCallbackOnHeaderField(http_parser* p, const char* at, size_t len);
      static int parserCallbackOnHeaderValue(http_parser* p, const char* at, size_t len);
      static int parserCallbackOnHeadersComplete(http_parser* p);
      static int parserCallbackOnBody(http_parser* p, const char* at, size_t len);
      static int parserCallbackOnMessageComplete(http_parser* p);

    public:
      http_parser parser;
      http_parser_settings parser_settings;
      int method;
      int version;
      HTTPURL url;
      HTTPHeaders headers;
      HTTPParameters parameters;
      HTTPParameters querystring;
      Buffer buffer_in;
    private:
      cb_request_read_body callback_read_body;
      void* callback_read_body_data;
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

    // SSL STATES
    /*
      enum HTTPOpenSSLState {
      SS_CONNECT_1 // connection just started (used while testing should be removed)
      ,SS_CONNECT_2 // used while testing, should be removed
      };
    */

    // HTTP CALLBACKS
    struct HTTPConnection;
    //typedef void(*http_cb_on_read)(HTTPConnection* c, void* ctx);
    typedef void(*http_cb_on_close)(HTTPConnection* c, void* ctx);
    typedef void(*http_cb_on_error)(HTTPConnection* c, void* ctx);

    // HTTP CONNECTION
    class HTTP;
    struct HTTPConnection {
      HTTPConnection(HTTP* http);
      ~HTTPConnection();
      void addToOutputBuffer(const std::string str);
      void addToOutputBuffer(const char* data, size_t len);
      void send(const char* data, size_t len);

      Buffer buffer_out;
      HTTPRequest response;
      int state; // used by "caller"
      HTTP* http;
      http_cb_on_close close_callback;
      http_cb_on_error error_callback;
      //http_cb_on_read read_callback;
      void* close_callback_data;
      void* error_callback_data;
      //void* read_callback_data;

      SSL* ssl;
      SSLBuffer ssl_buffer;

      uv_connect_t connect_req;
      uv_tcp_t socket;
      uv_write_t write_req;
      uv_getaddrinfo_t resolver_req;
    };

    // HTTP CONTEXT
    class HTTP {
    public:
      HTTP();
      ~HTTP();
      void update();

      HTTPConnection* sendRequest(
                                  HTTPRequest& r
                                  ,cb_request_read_body readCB = NULL
                                  ,void* readData = NULL
                                  ,http_cb_on_close closeCB = NULL
                                  ,void* closeData = NULL
                                  ,http_cb_on_error errorCB = NULL
                                  ,void* errorData = NULL
                                  );

      HTTPConnection* sendSecureRequest(
                                        SSL_CTX* ctx
                                        ,HTTPRequest& r
                                        ,cb_request_read_body readCB = NULL
                                        ,void* readData = NULL
                                        ,http_cb_on_close closeCB = NULL
                                        ,void* closeData = NULL
                                        ,http_cb_on_error errorCB = NULL
                                        ,void* errorData = NULL
                                        );
      HTTPConnection* newConnection(    
                                    cb_request_read_body readCB = NULL
                                    ,void* readData = NULL
                                    ,http_cb_on_close closeCB = NULL
                                    ,void* closeData = NULL
                                    ,http_cb_on_error errorCB = NULL
                                    ,void* errorData = NULL
                                        );

      static uv_buf_t callbackOnAlloc(uv_handle_t* con, size_t len);
      static void callbackOnResolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res);
      static void callbackOnConnect(uv_connect_t* con, int status);
      static void callbackOnRead(uv_stream_t* tcp, ssize_t nread, uv_buf_t buf);
      //  static void callbackOnWritten(uv_write_t* req, int status);
      static void callbackSSLMustWriteToSocket(const char* data, size_t len, void* con);
      static void callbackSSLReadDecryptedData(const char* data, size_t len, void* con);
    private:
      void writeToSocket(const char* data, size_t len);
      void removeConnection(HTTPConnection* c);
      void removeAllConnections();
    public:
      uv_loop_t* loop;
      std::vector<HTTPConnection*> connections;
    };
  } // roxlu
} // twitter
#endif
