#ifndef ROXLU_HTTP_REQUEST_H
#define ROXLU_HTTP_REQUEST_H

#include <http/HTTPTypes.h>
#include <http/HTTPHeaders.h>
#include <http/HTTPHeader.h>
#include <http/HTTPParameter.h>
#include <http/HTTPParameters.h>
#include <http/HTTPURL.h>

class HTTPRequest {

 public:
  HTTPRequest();
  HTTPRequest(HTTPURL url, HTTPMethod m = HTTP_METHOD_GET, HTTPVersion v = HTTP_VERSION_1_0);
  ~HTTPRequest();
  void addHeader(HTTPHeader h);                                      /* add a http header */
  void addHeaders(const HTTPHeaders& h);                            /* append the given headers to the headers member */
  void addContentParameter(HTTPParameter p);                         /* add a parameter that will be added into the content of the request (multipart/form-data, or url encoded) */
  void addContentParameters(HTTPParameters& p);                      /* "" */
  void setBody(std::string data);                                /* set the contents of the request body */
  void setURL(HTTPURL url);                                          /* set the url object */
  void setMethod(HTTPMethod method);                                 /* POST, GET, .. */
  void setVersion(HTTPVersion version);                              /* what proto version 1.1 or 1.0 */
  bool toString(std::string& result);                                /* converts the request into a string that can be sent to a http server */
  bool createBody(std::string& result);                              /* generates the contents of the request (which is put after the headers) */
  void addDefaultHTTPHeaders();                                      /* adds the GET or POST header and the form encoding type if necessary */

  std::string getHTTPString();                                       /* returns the first line for the request */
  bool isPost();                                                     /* helper which returns true when the current request will use POST */
  bool hasFileParameter();                                           /* helper which returns true when there are content parameters that are files */
  void generateBoundary();                                           /* generates an unique boundary string */
  std::string getBoundary();                                         /* returns the generated boundary string */ 
  HTTPURL& getURL();                                                 /* returns the URL with the host information to which we send the request */
  HTTPMethod getMethod();                                            /* POST, GET, .. */
  HTTPVersion getVersion();                                          /* HTTP 1.0 or 1.1 */
  HTTPFormEncoding getFormEncoding();                                /* returns the form encoding; if the current value is HTTP_FORM_ENCODING_NONE (default), we automatically detect the correct encoding */
  HTTPParameters& getContentParameters();                            /* returns a reference to the content parameters */
  void print();                                                      /* shows some debug info */

 private:
  HTTPURL url;
  HTTPMethod method;
  HTTPVersion version;
  HTTPHeaders headers;
  HTTPParameters content_parameters;
  HTTPFormEncoding form_encoding;                                   /* the form encoding in case of an post; if not set we detect it ourself */
  std::string boundary;                                             /* used as boundary string in multipart/form-data */
  std::string body;                                                 /* the body of the request */
};

inline void HTTPRequest::setURL(HTTPURL u) {
  url = u;
}

inline void HTTPRequest::setMethod(HTTPMethod m) {
  method = m;
}

inline void HTTPRequest::setVersion(HTTPVersion v) {
  version = v;
}

inline void HTTPRequest::setBody(std::string data) {
  body = data;
}

inline HTTPURL& HTTPRequest::getURL() {
  return url;
}

inline HTTPMethod HTTPRequest::getMethod() {
  return method;
}

inline HTTPVersion HTTPRequest::getVersion() {
  return version;
}

inline bool HTTPRequest::isPost() {
  return method == HTTP_METHOD_POST;
}

inline std::string HTTPRequest::getBoundary() {
  return boundary;
}

inline HTTPParameters& HTTPRequest::getContentParameters() {
  return content_parameters;
}

inline void HTTPRequest::addContentParameters(HTTPParameters& p) {
  content_parameters.copy(p);
}

inline void HTTPRequest::addHeaders(const HTTPHeaders& h) {
  headers.copy(h);
}

#endif
